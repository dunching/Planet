// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"
#include <atomic>

// Copyable atomic
template<typename T>
struct TVoxelAtomic
{
public:
	TVoxelAtomic()
	{
		Value = T{};
	}
	TVoxelAtomic(const T Value)
		: Value(Value)
	{
	}

	FORCEINLINE TVoxelAtomic(const TVoxelAtomic& Other)
	{
		Store(Other.Load());
	}
	FORCEINLINE TVoxelAtomic(TVoxelAtomic&& Other)
	{
		Store(Other.Load());
	}

public:
	FORCEINLINE TVoxelAtomic& operator=(const TVoxelAtomic& Other)
	{
		Store(Other.Load());
		return *this;
	}
	FORCEINLINE TVoxelAtomic& operator=(TVoxelAtomic&& Other)
	{
		Store(Other.Load());
		return *this;
	}

	template<typename Other>
	FORCEINLINE TVoxelAtomic& operator=(Other) = delete;

public:
	FORCEINLINE T Load(const std::memory_order MemoryOrder = std::memory_order_seq_cst) const
	{
		return Value.load(MemoryOrder);
	}
	FORCEINLINE void Store(const T NewValue, const std::memory_order MemoryOrder = std::memory_order_seq_cst)
	{
		Value.store(NewValue, MemoryOrder);
	}

	// Returns previous value
	FORCEINLINE T Exchange(const T NewValue, const std::memory_order MemoryOrder = std::memory_order_seq_cst)
	{
		return Value.exchange(NewValue, MemoryOrder);
	}
	// Returns true if exchange was successful
	// Expected will hold the old value (only useful if fails, if succeeds Expected == NewValue)
	FORCEINLINE bool CompareExchangeStrong(T& Expected, const T NewValue, const std::memory_order MemoryOrder = std::memory_order_seq_cst)
	{
		return Value.compare_exchange_strong(Expected, NewValue, MemoryOrder);
	}
	// Might spuriously fail
	FORCEINLINE bool CompareExchangeWeak(T& Expected, const T NewValue, const std::memory_order MemoryOrder = std::memory_order_seq_cst)
	{
		return Value.compare_exchange_weak(Expected, NewValue, MemoryOrder);
	}

private:
	std::atomic<T> Value;

	checkStatic(std::atomic<T>::is_always_lock_free);
};