// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"
#include "VoxelMinimal/VoxelAtomic.h"

class FVoxelSafeCriticalSection
{
public:
	FVoxelSafeCriticalSection() = default;

	FORCEINLINE void Lock()
	{
		check(LockerThreadId != ThreadId());

		Section.Lock();
		ensure(LockCounter.Set(1) == 0);

		check(LockerThreadId == -1);
		LockerThreadId = ThreadId();
	}
	FORCEINLINE void Unlock()
	{
		check(LockerThreadId == ThreadId());
		LockerThreadId = -1;

		ensure(LockCounter.Set(0) == 1);
		Section.Unlock();
	}

	FORCEINLINE bool IsLocked() const
	{
		return LockCounter.GetValue() > 0;
	}
	FORCEINLINE bool IsLockedByThisThread() const
	{
		return LockerThreadId == FPlatformTLS::GetCurrentThreadId();
	}

private:
	FCriticalSection Section;
	FThreadSafeCounter LockCounter;
	uint32 LockerThreadId = -1;

	FORCEINLINE static uint32 ThreadId()
	{
		return FPlatformTLS::GetCurrentThreadId();
	}
};

class FVoxelSafeSharedCriticalSection
{
public:
	FVoxelSafeSharedCriticalSection() = default;

	FORCEINLINE void ReadLock()
	{
		{
			FScopeLock Lock(&DebugSection);
			check(!Readers.Contains(ThreadId()));
			check(!Writers.Contains(ThreadId()));
		}
		Section.ReadLock();
		{
			FScopeLock Lock(&DebugSection);
			Readers.Add(ThreadId());
		}
	}
	FORCEINLINE void ReadUnlock()
	{
		Section.ReadUnlock();
		{
			FScopeLock Lock(&DebugSection);
			verify(Readers.Remove(ThreadId()));
		}
	}

	FORCEINLINE void WriteLock()
	{
		{
			FScopeLock Lock(&DebugSection);
			check(!Readers.Contains(ThreadId()));
			check(!Writers.Contains(ThreadId()));
		}
		Section.WriteLock();
		{
			FScopeLock Lock(&DebugSection);
			Writers.Add(ThreadId());
		}
	}
	FORCEINLINE void WriteUnlock()
	{
		Section.WriteUnlock();
		{
			FScopeLock Lock(&DebugSection);
			verify(Writers.Remove(ThreadId()));
		}
	}

	FORCEINLINE bool IsLocked_Read() const
	{
		FScopeLock Lock(&DebugSection);
		return Readers.Num() > 0 || Writers.Num() > 0;
	}
	FORCEINLINE bool IsLocked_Write() const
	{
		FScopeLock Lock(&DebugSection);
		return Writers.Num() > 0;
	}

private:
	FRWLock Section;
	mutable FCriticalSection DebugSection;
	TSet<uint32> Readers;
	TSet<uint32> Writers;

	FORCEINLINE static uint32 ThreadId()
	{
		return FPlatformTLS::GetCurrentThreadId();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class FVoxelCriticalSection
{
public:
	FVoxelCriticalSection() = default;

	// Allow copying for convenience
	FVoxelCriticalSection(const FVoxelCriticalSection&)
	{
	}
	FVoxelCriticalSection& operator=(const FVoxelCriticalSection&)
	{
		return *this;
	}

	FORCEINLINE void Lock()
	{
		Section.Lock();
	}
	FORCEINLINE void Unlock()
	{
		Section.Unlock();
	}

	FORCEINLINE bool ShouldRecordStats() const
	{
		return true;
	}
#if VOXEL_DEBUG
	bool IsLocked_Debug() const
	{
		return Section.IsLocked();
	}
	FORCEINLINE bool IsLockedByThisThread_Debug() const
	{
		return Section.IsLockedByThisThread();
	}
#endif

private:
#if VOXEL_DEBUG
	FVoxelSafeCriticalSection Section;
#else
	FCriticalSection Section;
#endif
};

class FVoxelSharedCriticalSection
{
public:
	FVoxelSharedCriticalSection() = default;

	// Allow copying for convenience
	FVoxelSharedCriticalSection(const FVoxelSharedCriticalSection&)
	{
	}
	FVoxelSharedCriticalSection& operator=(const FVoxelSharedCriticalSection&)
	{
		return *this;
	}

	FORCEINLINE void ReadLock()
	{
		Section.ReadLock();
	}
	FORCEINLINE void ReadUnlock()
	{
		Section.ReadUnlock();
	}

	FORCEINLINE void WriteLock()
	{
		Section.WriteLock();
	}
	FORCEINLINE void WriteUnlock()
	{
		Section.WriteUnlock();
	}

	FORCEINLINE bool ShouldRecordStats() const
	{
		return true;
	}
#if VOXEL_DEBUG
	FORCEINLINE bool IsLocked_Read_Debug() const
	{
		return Section.IsLocked_Read();
	}
	FORCEINLINE bool IsLocked_Write_Debug() const
	{
		return Section.IsLocked_Write();
	}
#endif

private:
#if VOXEL_DEBUG
	FVoxelSafeSharedCriticalSection Section;
#else
	FRWLock Section;
#endif
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct FVoxelCacheLinePadding
{
public:
	FORCEINLINE FVoxelCacheLinePadding()
	{
	}
	FORCEINLINE FVoxelCacheLinePadding(const FVoxelCacheLinePadding&)
	{
	}
	FORCEINLINE FVoxelCacheLinePadding(FVoxelCacheLinePadding&&)
	{
	}
	FORCEINLINE FVoxelCacheLinePadding& operator=(const FVoxelCacheLinePadding&)
	{
		return *this;
	}
	FORCEINLINE FVoxelCacheLinePadding& operator=(FVoxelCacheLinePadding&&)
	{
		return *this;
	}

private:
	uint8 Padding[PLATFORM_CACHE_LINE_SIZE * 2];
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename StorageType>
class TVoxelFastCriticalSectionImpl
{
public:
	TVoxelFastCriticalSectionImpl() = default;

	FORCEINLINE void Lock()
	{
		checkVoxelSlow(Storage.LockerThreadId.Load() != FPlatformTLS::GetCurrentThreadId());

		while (true)
		{
			if (Storage.bIsLocked.Exchange(true, std::memory_order_acquire) == false)
			{
				break;
			}

			while (Storage.bIsLocked.Load(std::memory_order_relaxed) == true)
			{
				FPlatformProcess::Yield();
			}
		}

		checkVoxelSlow(Storage.LockerThreadId.Load() == 0);
		VOXEL_DEBUG_ONLY(Storage.LockerThreadId.Store(FPlatformTLS::GetCurrentThreadId()));
	}
	FORCEINLINE void Unlock()
	{
		checkVoxelSlow(Storage.LockerThreadId.Load() == FPlatformTLS::GetCurrentThreadId());
		VOXEL_DEBUG_ONLY(Storage.LockerThreadId.Store(0));

		Storage.bIsLocked.Store(false, std::memory_order_release);
	}

	FORCEINLINE bool IsLocked() const
	{
		return Storage.bIsLocked.Load(std::memory_order_relaxed);
	}
	FORCEINLINE bool ShouldRecordStats() const
	{
		return IsLocked();
	}
#if VOXEL_DEBUG
	FORCEINLINE bool IsLockedByThisThread_Debug() const
	{
		return Storage.LockerThreadId.Load() == FPlatformTLS::GetCurrentThreadId();
	}
#endif

private:
	StorageType Storage;
};

namespace Impl
{
	struct FVoxelFastCriticalSectionStorage
	{
		FVoxelCacheLinePadding PaddingA;
		TVoxelAtomic<bool> bIsLocked = false;
#if VOXEL_DEBUG
		TVoxelAtomic<uint32> LockerThreadId = 0;
#endif
		FVoxelCacheLinePadding PaddingB;
	};

	struct FVoxelFastCriticalSectionStorage_NoPadding
	{
		TVoxelAtomic<bool> bIsLocked = false;
#if VOXEL_DEBUG
		TVoxelAtomic<uint32> LockerThreadId = 0;
#endif
	};
}

using FVoxelFastCriticalSection = TVoxelFastCriticalSectionImpl<Impl::FVoxelFastCriticalSectionStorage>;
using FVoxelFastCriticalSection_NoPadding = TVoxelFastCriticalSectionImpl<Impl::FVoxelFastCriticalSectionStorage_NoPadding>;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class FVoxelScopeLock_Read
{
public:
	FORCEINLINE explicit FVoxelScopeLock_Read(FVoxelSharedCriticalSection& Section)
		: Section(Section)
	{
		VOXEL_FUNCTION_COUNTER();
		Section.ReadLock();
	}
	FORCEINLINE ~FVoxelScopeLock_Read()
	{
		Section.ReadUnlock();
	}

private:
	FVoxelSharedCriticalSection& Section;
};

class FVoxelScopeLock_Write
{
public:
	FORCEINLINE explicit FVoxelScopeLock_Write(FVoxelSharedCriticalSection& Section)
		: Section(Section)
	{
		VOXEL_FUNCTION_COUNTER();
		Section.WriteLock();
	}
	FORCEINLINE ~FVoxelScopeLock_Write()
	{
		Section.WriteUnlock();
	}

private:
	FVoxelSharedCriticalSection& Section;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define VOXEL_SCOPE_LOCK(...) \
	{ \
		VOXEL_SCOPE_COUNTER_COND((__VA_ARGS__).ShouldRecordStats(), "Lock " #__VA_ARGS__); \
		(__VA_ARGS__).Lock(); \
	} \
	ON_SCOPE_EXIT \
	{ \
		(__VA_ARGS__).Unlock(); \
	};