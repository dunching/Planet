// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"
#include "VoxelMinimal/Utilities/VoxelBaseUtilities.h"
#include "VoxelMinimal/Containers/VoxelBitArrayHelpers.h"

template<typename T>
class TVoxelPackedArrayView
{
public:
	static_assert(std::is_same_v<std::remove_const_t<T>, uint32>, "");
	static constexpr int32 NumBitsPerWord = 32;

	TVoxelPackedArrayView() = default;
	TVoxelPackedArrayView(int32 NumBits, T* RESTRICT Data, int32 NumElements)
		: NumBits(NumBits)
		, ArrayData(Data)
		, ArrayNum(NumElements)
	{
		check(NumBits <= sizeof(T) * 8);
	}
	template<typename Array>
	TVoxelPackedArrayView(int32 NumBits, Array& InArray, int32 NumElements, int32 WordOffset = 0)
		: TVoxelPackedArrayView(NumBits, InArray.GetData() + WordOffset, NumElements)
	{
		if (NumElements > 0)
		{
			check(InArray.IsValidIndex(WordOffset));
			check(InArray.IsValidIndex(WordOffset + FVoxelUtilities::DivideCeil_Positive(NumElements * NumBits - 1, NumBitsPerWord)));
		}
	}

	template<typename ArrayView>
	class TAccessor
	{
	public:
		TAccessor(ArrayView& Array, int32 Index)
			: Array(Array)
			, Index(Index)
		{
		}

		FORCEINLINE operator T() const
		{
			return Array.Get(Index);
		}
		FORCEINLINE void operator=(T NewValue)
		{
			Array.Set(Index, NewValue);
		}

	private:
		ArrayView& Array;
		const int32 Index;
	};

	TAccessor<TVoxelPackedArrayView> operator[](int32 Index)
	{
		return { *this, Index };
	}
	TAccessor<const TVoxelPackedArrayView> operator[](int32 Index) const
	{
		return { *this, Index };
	}

	FORCEINLINE uint32 Get(int32 Index) const
	{
		checkVoxelSlow(IsValidIndex(Index));
		return FVoxelBitArrayHelpers::GetPacked(NumBits, ArrayData, Index);

	}
	FORCEINLINE void Set(int32 Index, uint32 Value) const
	{
		checkVoxelSlow(IsValidIndex(Index));
		FVoxelBitArrayHelpers::SetPacked(NumBits, ArrayData, Index, Value);
		checkVoxelSlow(Get(Index) == Value);
	}

	int32 Num() const
	{
		return ArrayNum;
	}
	bool IsValidIndex(int32 Index) const
	{
		return 0 <= Index && Index < ArrayNum;
	}

private:
	int32 NumBits = 0;
	T* RESTRICT ArrayData = nullptr;
	// Not the size of ArrayData, but the number of T in it
	int32 ArrayNum = 0;
};