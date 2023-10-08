// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"
#include "VoxelMinimal/Containers/VoxelPackedArrayView.h"

template<typename T>
class TVoxelPackedData : public T
{
public:
	using EDataType = typename T::EDataType;

	static EDataType Previous(EDataType DataType)
	{
		checkVoxelSlow(int32(DataType) != 0);
		return EDataType(int32(DataType) - 1);
	}

public:
	int32 NumWords(EDataType DataType) const
	{
		return FVoxelUtilities::DivideCeil_Positive(this->Num(DataType) * this->NumBits(DataType), 32);
	}
	int32 Offset(EDataType DataType) const
	{
		if (int32(DataType) == 0)
		{
			return 0;
		}
		else
		{
			return Offset(Previous(DataType)) + NumWords(Previous(DataType));
		}
	}
	int32 NumWords() const
	{
		return Offset(EDataType::Count);
	}

public:
	TVoxelPackedArrayView<uint32> GetView(EDataType DataType)
	{
		return { this->NumBits(DataType), &Data[Offset(DataType)], this->Num(DataType) };
	}
	TVoxelPackedArrayView<const uint32> GetView(EDataType DataType) const
	{
		return { this->NumBits(DataType), &Data[Offset(DataType)], this->Num(DataType) };
	}

public:
	using T::T;

	void Initialize()
	{
		check(Data.Num() == 0);
		Data.Empty(NumWords());
		Data.SetNumUninitialized(NumWords());
	}
	int64 GetAllocatedSize() const
	{
		return Data.GetAllocatedSize();
	}

private:
	TArray<uint32> Data;
};