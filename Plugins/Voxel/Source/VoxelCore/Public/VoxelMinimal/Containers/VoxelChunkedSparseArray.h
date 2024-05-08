// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"
#include "VoxelMinimal/Containers/VoxelBitArray.h"
#include "VoxelMinimal/Containers/VoxelChunkedArray.h"

template<typename Type, int32 MaxBytesPerChunk = GVoxelDefaultAllocationSize>
class TVoxelChunkedSparseArray
{
public:
	TVoxelChunkedSparseArray() = default;
	FORCEINLINE ~TVoxelChunkedSparseArray()
	{
		Empty();
	}

	FORCEINLINE void Reserve(const int32 Number)
	{
		Array.Reserve(Number);
		AllocationFlags.Reserve(Number);
	}
	FORCEINLINE void Empty()
	{
		if (!TIsTriviallyDestructible<Type>::Value)
		{
			AllocationFlags.ForAllSetBits([&](const int32 Index)
			{
				ReinterpretCastRef<Type>(Array[Index].Value).~Type();
			});
		}

		ArrayNum = 0;
		FirstFreeIndex = -1;
		Array.Empty();
		AllocationFlags.Empty();
	}
	FORCEINLINE void Shrink()
	{
		Array.Shrink();
		AllocationFlags.Shrink();
	}
	FORCEINLINE int32 Num() const
	{
		checkVoxelSlow(ArrayNum == AllocationFlags.CountSetBits());
		return ArrayNum;
	}
	FORCEINLINE int64 GetAllocatedSize() const
	{
		return
			Array.GetAllocatedSize() +
			AllocationFlags.GetAllocatedSize();
	}
	FORCEINLINE bool IsValidIndex(const int32 Index) const
	{
		checkVoxelSlow(Array.Num() == AllocationFlags.Num());
		return Array.IsValidIndex(Index) && AllocationFlags[Index];
	}

	template<typename LambdaType>
	FORCEINLINE void Foreach(LambdaType Lambda)
	{
		AllocationFlags.ForAllSetBits([&](const int32 Index)
		{
			Lambda((*this)[Index]);
		});
	}
	template<typename LambdaType>
	FORCEINLINE void Foreach(LambdaType Lambda) const
	{
		AllocationFlags.ForAllSetBits([&](const int32 Index)
		{
			Lambda((*this)[Index]);
		});
	}

public:
	FORCEINLINE int32 AddUninitialized()
	{
		ArrayNum++;

		if (FirstFreeIndex != -1)
		{
			const int32 Result = FirstFreeIndex;
			FirstFreeIndex = Array[Result].NextFreeIndex;

			checkVoxelSlow(!AllocationFlags[Result]);
			AllocationFlags[Result] = true;

			return Result;
		}

		const int32 ArrayIndex = Array.AddUninitialized();
		const int32 BitIndex = AllocationFlags.Add(true);
		checkVoxelSlow(ArrayIndex == BitIndex);

		return ArrayIndex;
	}

	FORCEINLINE int32 Add(const Type& Value)
	{
		const int32 Index = AddUninitialized();
		new (&(*this)[Index]) Type(Value);
		return Index;
	}
	FORCEINLINE int32 Add(Type&& Value)
	{
		const int32 Index = AddUninitialized();
		new (&(*this)[Index]) Type(MoveTemp(Value));
		return Index;
	}

	FORCEINLINE Type& Emplace_GetRef()
	{
		const int32 Index = AddUninitialized();
		Type& Value = (*this)[Index];
		new (&Value) Type(MoveTemp(Value));
		return Value;
	}

	FORCEINLINE void RemoveAt(const int32 Index)
	{
		ArrayNum--;
		checkVoxelSlow(ArrayNum >= 0);

		checkVoxelSlow(IsValidIndex(Index));
		AllocationFlags[Index] = false;

		ReinterpretCastRef<Type>(Array[Index].Value).~Type();

		Array[Index].NextFreeIndex = FirstFreeIndex;
		FirstFreeIndex = Index;
	}

public:
	FORCEINLINE Type& operator[](const int32 Index)
	{
		checkVoxelSlow(IsValidIndex(Index));
		return ReinterpretCastRef<Type>(Array[Index].Value);
	}
	FORCEINLINE const Type& operator[](const int32 Index) const
	{
		return ConstCast(this)->operator[](Index);
	}

private:
	union FElement
	{
		TTypeCompatibleBytes<Type> Value;
		int32 NextFreeIndex = -1;
	};
	int32 ArrayNum = 0;
	int32 FirstFreeIndex = -1;
	FVoxelBitArray32 AllocationFlags;
	TVoxelChunkedArray<FElement, MaxBytesPerChunk> Array;
};