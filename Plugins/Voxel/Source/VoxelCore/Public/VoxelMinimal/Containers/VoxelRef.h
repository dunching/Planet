// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"

template<typename ValueType, typename StorageType, typename IndexType>
class TVoxelRef
{
public:
	TVoxelRef(StorageType& Storage, IndexType Index)
		: Storage(Storage)
		, Index(Index)
	{
	}

	FORCEINLINE operator ValueType() const
	{
		return Storage.Get(Index);
	}
	FORCEINLINE void operator=(ValueType NewValue)
	{
		Storage.Set(Index, NewValue);
	}
	FORCEINLINE TVoxelRef& operator=(const TVoxelRef& Copy)
	{
		// As this is emulating a reference, assignment should not rebind,
		// it should write to the referenced value
		*this = ValueType(Copy);
		return *this;
	}

	FORCEINLINE bool operator==(const ValueType& Other) const
	{
		return ValueType(*this) == Other;
	}
	FORCEINLINE bool operator!=(const ValueType& Other) const
	{
		return ValueType(*this) != Other;
	}

private:
	StorageType& Storage;
	const IndexType Index;
};

template<typename ValueType, typename StorageType, typename IndexType>
class TVoxelConstRef
{
public:
	TVoxelConstRef(const StorageType& Storage, IndexType Index)
		: Storage(Storage)
		, Index(Index)
	{
	}

	FORCEINLINE operator ValueType() const
	{
		return Storage.Get(Index);
	}

	FORCEINLINE bool operator==(const ValueType& Other) const
	{
		return ValueType(*this) == Other;
	}
	FORCEINLINE bool operator!=(const ValueType& Other) const
	{
		return ValueType(*this) != Other;
	}


private:
	const StorageType& Storage;
	const IndexType Index;
};