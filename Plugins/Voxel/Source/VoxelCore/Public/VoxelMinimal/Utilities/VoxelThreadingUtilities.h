// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"
#include "Async/ParallelFor.h"

template<ENamedThreads::Type Thread, ESubsequentsMode::Type SubsequentsMode = ESubsequentsMode::FireAndForget>
class TVoxelGraphTask
{
public:
	TVoxelUniqueFunction<void()> Lambda;

	explicit TVoxelGraphTask(TVoxelUniqueFunction<void()>&& Lambda)
		: Lambda(MoveTemp(Lambda))
	{
	}

	void DoTask(ENamedThreads::Type, const FGraphEventRef&) const
	{
		VOXEL_SCOPE_COUNTER("TVoxelGraphTask");
		Lambda();
	}

	static TStatId GetStatId()
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(TVoxelGraphTask, STATGROUP_Voxel);
	}
	static ENamedThreads::Type GetDesiredThread()
	{
		return Thread;
	}
	static ESubsequentsMode::Type GetSubsequentsMode()
	{
		return SubsequentsMode;
	}
};

FORCEINLINE bool IsInGameThreadFast()
{
	const bool bIsInGameThread = FPlatformTLS::GetCurrentThreadId() == GGameThreadId;
	ensureVoxelSlowNoSideEffects(bIsInGameThread == IsInGameThread());
	return bIsInGameThread;
}

VOXELCORE_API void FlushVoxelGameThreadTasks();
VOXELCORE_API void AsyncVoxelTask(TVoxelUniqueFunction<void()>&& Lambda);

namespace FVoxelUtilities
{
	// Will never be called right away, even if we are on the game thread
	// Useful to avoid deadlocks
	VOXELCORE_API void RunOnGameThread_Async(TVoxelUniqueFunction<void()>&& Lambda);

	template<typename T>
	void RunOnGameThread(T&& Lambda)
	{
		if (IsInGameThreadFast())
		{
			Lambda();
		}
		else
		{
			FVoxelUtilities::RunOnGameThread_Async(MoveTemp(Lambda));
		}
	}

	template<typename T>
	struct TGameThreadDeleter
	{
		void operator()(T* Object) const
		{
			if (!Object) return;

			FVoxelUtilities::RunOnGameThread([=]
			{
				delete Object;
			});
		}

		template<typename... TArgs>
		static TSharedRef<T> New(TArgs&&... Args)
		{
			return TSharedPtr<T>(new T(Forward<TArgs>(Args)...), TGameThreadDeleter()).ToSharedRef();
		}
	};

	template<typename KeyType, typename ValueType, typename SetAllocator, typename KeyFuncs>
	struct TMapWithPairs : TMap<KeyType, ValueType, SetAllocator, KeyFuncs>
	{
		auto& GetPairs()
		{
			return TMap<KeyType, ValueType, SetAllocator, KeyFuncs>::Pairs;
		}
	};
}

template<typename T, typename... TArgs>
TSharedRef<T> MakeShared_GameThread(TArgs&&... Args)
{
	return FVoxelUtilities::TGameThreadDeleter<T>::New(Forward<TArgs>(Args)...);
}

struct CParallelForLambdaHasIndex
{
	template<typename LambdaType, typename ValueType>
	auto Requires(LambdaType Lambda, ValueType Value, int32 Index) -> decltype(Lambda(Value, Index));
};

template<typename ArrayType, typename LambdaType>
typename TEnableIf<sizeof(GetNum(DeclVal<ArrayType>())) != 0>::Type ParallelFor(
	ArrayType& Array,
	LambdaType Lambda,
	const EParallelForFlags Flags = EParallelForFlags::None,
	const int32 DefaultNumThreads = FTaskGraphInterface::Get().GetNumWorkerThreads())
{
	VOXEL_ALLOW_MALLOC_SCOPE();

	const int64 ArrayNum = GetNum(Array);
	const int64 NumThreads = FMath::Clamp<int64>(DefaultNumThreads, 1, ArrayNum);
	ParallelFor(NumThreads, [&](const int64 ThreadIndex)
	{
		const int64 ElementsPerThreads = FVoxelUtilities::DivideCeil_Positive(ArrayNum, NumThreads);

		const int64 StartIndex = ThreadIndex * ElementsPerThreads;
		const int64 EndIndex = FMath::Min((ThreadIndex + 1) * ElementsPerThreads, ArrayNum);

		for (int64 ElementIndex = StartIndex; ElementIndex < EndIndex; ElementIndex++)
		{
			if constexpr (TModels<CParallelForLambdaHasIndex, LambdaType, decltype(Array[ElementIndex])>::Value)
			{
				Lambda(Array[ElementIndex], ElementIndex);
			}
			else
			{
				Lambda(Array[ElementIndex]);
			}
		}
	}, Flags);
}