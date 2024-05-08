// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"
#include "VoxelMinimal/Containers/VoxelRef.h"
#include "VoxelMinimal/Containers/VoxelBitArrayHelpers.h"

class FVoxelPackedArray
{
public:
	FVoxelPackedArray() = default;
	FVoxelPackedArray(int32 BitsPerElement, int32 ArrayNum)
		: BitsPerElement(BitsPerElement)
		, ArrayNum(ArrayNum)
		, Data(AllocateData(NumWords()))
	{
	}
	FVoxelPackedArray(const FVoxelPackedArray& Other)
		: BitsPerElement(Other.BitsPerElement)
		, ArrayNum(Other.ArrayNum)
		, Data(AllocateData(NumWords()))
	{
		FMemory::Memcpy(GetData(), Other.GetData(), NumWords() * sizeof(uint32));
	}
	FVoxelPackedArray(FVoxelPackedArray&& Other)
		: BitsPerElement(Other.BitsPerElement)
		, ArrayNum(Other.ArrayNum)
		, Data(Other.Data)
	{
		Other.BitsPerElement = 0;
		Other.ArrayNum = 0;
		Other.Data = nullptr;
	}
	~FVoxelPackedArray()
	{
		FMemory::Free(Data);
	}

	FVoxelPackedArray& operator=(const FVoxelPackedArray& Other)
	{
		FMemory::Free(Data);

		BitsPerElement = Other.BitsPerElement;
		ArrayNum = Other.ArrayNum;
		Data = AllocateData(NumWords());

		FMemory::Memcpy(GetData(), Other.GetData(), NumWords() * sizeof(uint32));

		return *this;
	}
	FVoxelPackedArray& operator=(FVoxelPackedArray&& Other)
	{
		FMemory::Free(Data);

		BitsPerElement = Other.BitsPerElement;
		ArrayNum = Other.ArrayNum;
		Data = Other.Data;

		Other.BitsPerElement = 0;
		Other.ArrayNum = 0;
		Other.Data = nullptr;

		return *this;
	}

public:
	void Initialize(int32 InBitsPerElement, int32 InArrayNum)
	{
		*this = FVoxelPackedArray(InBitsPerElement, InArrayNum);
	}
	void Reset()
	{
		*this = {};
	}
	void Memzero()
	{
		if (Data)
		{
			FMemory::Memzero(GetData(), NumWords() * sizeof(uint32));
		}
	}

public:
	FORCEINLINE int32 GetBitsPerElement() const
	{
		return BitsPerElement;
	}
	FORCEINLINE int32 Num() const
	{
		return ArrayNum;
	}
	FORCEINLINE int32 NumWords() const
	{
		checkVoxelSlow(BitsPerElement > 0 || ArrayNum == 0);
		return FVoxelUtilities::DivideCeil_Positive(ArrayNum * BitsPerElement, 32);
	}
	FORCEINLINE int64 GetAllocatedSize() const
	{
		return NumWords() * sizeof(uint32);
	}
	FORCEINLINE bool IsValidIndex(int32 Index) const
	{
		return 0 <= Index && Index < ArrayNum;
	}

	FORCEINLINE uint32* RESTRICT GetData()
	{
		return Data;
	}
	FORCEINLINE const uint32* RESTRICT GetData() const
	{
		return Data;
	}

	FORCEINLINE TVoxelRef<uint32, FVoxelPackedArray, int32> operator[](int32 Index)
	{
		return { *this, Index };
	}
	FORCEINLINE TVoxelConstRef<uint32, FVoxelPackedArray, int32> operator[](int32 Index) const
	{
		return { *this, Index };
	}

	FORCEINLINE uint32 Get(int32 Index) const
	{
		checkVoxelSlow(IsValidIndex(Index));
		return FVoxelBitArrayHelpers::GetPacked(BitsPerElement, GetData(), Index);

	}
	FORCEINLINE void Set(int32 Index, uint32 Value)
	{
		checkVoxelSlow(IsValidIndex(Index));
		FVoxelBitArrayHelpers::SetPacked(BitsPerElement, GetData(), Index, Value);
		checkVoxelSlow(Get(Index) == Value);
	}

	template<int32 StaticBitsPerElement>
	FORCEINLINE uint32 GetStatic(int32 Index) const
	{
		checkVoxelSlow(IsValidIndex(Index));
		return FVoxelBitArrayHelpers::GetPacked(StaticBitsPerElement, GetData(), Index);

	}
	template<int32 StaticBitsPerElement>
	FORCEINLINE void SetStatic(int32 Index, uint32 Value)
	{
		checkVoxelSlow(IsValidIndex(Index));
		checkVoxelSlow(BitsPerElement == StaticBitsPerElement);
		FVoxelBitArrayHelpers::SetPacked(StaticBitsPerElement, GetData(), Index, Value);
		checkVoxelSlow(Get(Index) == Value);
	}

public:
	friend FArchive& operator<<(FArchive& Ar, FVoxelPackedArray& Array)
	{
		checkVoxelSlow(0 <= Array.BitsPerElement && Array.BitsPerElement <= 32);
		uint8 BitsPerElementByte = Array.BitsPerElement;
		Ar << BitsPerElementByte;
		Array.BitsPerElement = BitsPerElementByte;

		Ar << Array.ArrayNum;

		if (Ar.IsLoading())
		{
			FMemory::Free(Array.Data);
			Array.Data = AllocateData(Array.NumWords());
		}

		Ar.Serialize(Array.GetData(), Array.NumWords() * sizeof(uint32));

		return Ar;
	}

private:
	int32 BitsPerElement = 0;
	int32 ArrayNum = 0;
	uint32* RESTRICT Data = nullptr;

	static uint32* AllocateData(int32 WordsCount)
	{
		if (WordsCount > 0)
		{
			return static_cast<uint32*>(FMemory::Malloc(WordsCount * sizeof(uint32)));
		}
		else
		{
			return nullptr;
		}
	}
};