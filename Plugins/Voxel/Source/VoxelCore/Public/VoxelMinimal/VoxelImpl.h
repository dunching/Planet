// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"

template<typename T>
class TVoxelImpl
{
public:
	struct FInit
	{
		T* Ptr = nullptr;
		void (*DeleteFunc)(T*) = nullptr;
	};

	FORCEINLINE explicit TVoxelImpl(const FInit& Init)
		: Ptr(Init.Ptr)
		, DeleteFunc(Init.DeleteFunc)
	{
		checkVoxelSlow(Ptr);
		checkVoxelSlow(DeleteFunc);
	}
	FORCEINLINE ~TVoxelImpl()
	{
		checkVoxelSlow(Ptr);
		checkVoxelSlow(DeleteFunc);
		(*DeleteFunc)(Ptr);
	}
	UE_NONCOPYABLE(TVoxelImpl);

	FORCEINLINE T* operator->()
	{
		checkVoxelSlow(Ptr);
		return Ptr;
	}
	FORCEINLINE T& operator*()
	{
		checkVoxelSlow(Ptr);
		return *Ptr;
	}

	FORCEINLINE const T* operator->() const
	{
		checkVoxelSlow(Ptr);
		return Ptr;
	}
	FORCEINLINE const T& operator*() const
	{
		checkVoxelSlow(Ptr);
		return *Ptr;
	}

private:
	T* Ptr = nullptr;
	void (*DeleteFunc)(T*) = nullptr;
};

template<typename T, typename... ArgTypes>
FORCEINLINE typename TVoxelImpl<T>::FInit MakeVoxelImpl(ArgTypes&&... Args)
{
	typename TVoxelImpl<T>::FInit Init;
	Init.Ptr = new (GVoxelMemory) T(Forward<ArgTypes>(Args)...);
	Init.DeleteFunc = [](T* Value)
	{
		Value->~T();
		FVoxelMemory::Free(Value);
	};
	return Init;
}

#define DECLARE_VOXEL_IMPL() \
	struct FImpl; \
	TVoxelImpl<FImpl> Impl;