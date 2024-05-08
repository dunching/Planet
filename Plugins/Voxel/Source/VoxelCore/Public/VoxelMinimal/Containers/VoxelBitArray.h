// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"
#include "VoxelMinimal/Containers/VoxelBitArrayHelpers.h"

class FVoxelBitReference
{
public:
	FORCEINLINE FVoxelBitReference(uint32& Data, uint32 Mask)
		: Data(Data)
		, Mask(Mask)
	{
	}

	FORCEINLINE operator bool() const
	{
		return (Data & Mask) != 0;
	}
	FORCEINLINE void operator=(const bool NewValue)
	{
		if (NewValue)
		{
			Data |= Mask;
		}
		else
		{
			Data &= ~Mask;
		}
	}
	FORCEINLINE void operator|=(const bool NewValue)
	{
		if (NewValue)
		{
			Data |= Mask;
		}
	}
	FORCEINLINE void operator&=(const bool NewValue)
	{
		if (!NewValue)
		{
			Data &= ~Mask;
		}
	}
	FORCEINLINE FVoxelBitReference& operator=(const FVoxelBitReference& Copy)
	{
		// As this is emulating a reference, assignment should not rebind,
		// it should write to the referenced bit.
		*this = bool(Copy);
		return *this;
	}

private:
	uint32& Data;
	uint32 Mask;
};

class FVoxelConstBitReference
{
public:
	FORCEINLINE FVoxelConstBitReference(const uint32& Data, uint32 Mask)
		: Data(Data)
		, Mask(Mask)
	{

	}

	FORCEINLINE operator bool() const
	{
		return (Data & Mask) != 0;
	}

private:
	const uint32& Data;
	uint32 Mask;
};

// Tricky: we need to ensure the last word is always zero-padded
template<typename Allocator>
class TVoxelBitArray
{
public:
	typedef typename Allocator::SizeType SizeType;

	static constexpr SizeType NumBitsPerWord = 32;
	static constexpr SizeType NumBitsPerWordLog2 = 5;

	template<typename>
	friend class TVoxelBitArray;

	TVoxelBitArray() = default;

	FORCEINLINE TVoxelBitArray(TVoxelBitArray&& Other)
	{
		*this = MoveTemp(Other);
	}
	FORCEINLINE TVoxelBitArray(const TVoxelBitArray& Other)
	{
		*this = Other;
	}

	FORCEINLINE TVoxelBitArray& operator=(TVoxelBitArray&& Other)
	{
		checkVoxelSlow(this != &Other);

		AllocatorInstance.MoveToEmpty(Other.AllocatorInstance);

		NumBits = Other.NumBits;
		MaxBits = Other.MaxBits;
		Other.NumBits = 0;
		Other.MaxBits = 0;

		EnsurePartialSlackBitsCleared();

		return *this;
	}
	FORCEINLINE TVoxelBitArray& operator=(const TVoxelBitArray& Other)
	{
		checkVoxelSlow(this != &Other);

		NumBits = Other.NumBits;
		SetMaxBits(Other.NumBits);

		if (NumWords() != 0)
		{
			FMemory::Memcpy(GetWordData(), Other.GetWordData(), NumWords() * sizeof(uint32));
		}

		EnsurePartialSlackBitsCleared();

		return *this;
	}

public:
	template<typename OtherAllocator>
	FORCEINLINE bool operator==(const TVoxelBitArray<OtherAllocator>& Other) const
	{
		if (Num() != Other.Num())
		{
			return false;
		}

		return FVoxelUtilities::MemoryEqual(GetWordData(), Other.GetWordData(), NumWords() * sizeof(uint32));
	}
	template<typename OtherAllocator>
	FORCEINLINE bool operator!=(const TVoxelBitArray<OtherAllocator>& Other)
	{
		return !(*this == Other);
	}

public:
	friend FArchive& operator<<(FArchive& Ar, TVoxelBitArray& BitArray)
	{
		Ar << BitArray.NumBits;

		if (Ar.IsLoading())
		{
			BitArray.SetMaxBits(BitArray.NumBits);
		}

		Ar.Serialize(BitArray.GetWordData(), BitArray.NumWords() * sizeof(uint32));
		BitArray.EnsurePartialSlackBitsCleared();

		return Ar;
	}
	FORCEINLINE void Reserve(SizeType NewMaxBits)
	{
		if (NewMaxBits <= MaxBits)
		{
			return;
		}

		SetMaxBits(NewMaxBits);
	}
	FORCEINLINE void Empty(SizeType NewMaxBits = 0)
	{
		NumBits = 0;
		SetMaxBits(NewMaxBits);
	}
	FORCEINLINE void Shrink()
	{
		SetMaxBits(NumBits);
	}
	FORCEINLINE void Reset()
	{
		NumBits = 0;
	}

	// No SetNumUninitialized: the last word must always be zero-padded
	void SetNum(SizeType NewNumBits, bool bValue)
	{
		// Has different behavior than TArray::SetNum
		checkVoxelSlow(Num() == 0);

		NumBits = NewNumBits;
		SetMaxBits(FMath::Max(MaxBits, NewNumBits));

		if (NumWords() != 0)
		{
			FMemory::Memset(GetWordData(), bValue ? 0xFF : 0, NumWords() * sizeof(uint32));
		}

		ClearPartialSlackBits();
	}
	void SetNumZeroed(SizeType InNumBits)
	{
		this->SetNum(InNumBits, false);
	}

	FORCEINLINE void SetRange(SizeType Index, SizeType Num, bool Value)
	{
		FVoxelBitArrayHelpers::SetRange(*this, Index, Num, Value);
	}
	FORCEINLINE bool TestAndClear(uint32 Index)
	{
		return FVoxelBitArrayHelpers::TestAndClear(*this, Index);
	}
	FORCEINLINE bool TestRange(uint32 Index, uint32 Num) const
	{
		return FVoxelBitArrayHelpers::TestRange(*this, Index, Num);
	}
	// Test a range, and clears it if all true
	FORCEINLINE bool TestAndClearRange(uint32 Index, uint32 Num)
	{
		return FVoxelBitArrayHelpers::TestAndClearRange(*this, Index, Num);
	}

public:
	FORCEINLINE TOptional<bool> TryGetAll() const
	{
		return FVoxelBitArrayHelpers::TryGetAll(*this);
	}
	FORCEINLINE bool AllEqual(bool bValue) const
	{
		return FVoxelBitArrayHelpers::AllEqual(*this, bValue);
	}
	FORCEINLINE SizeType CountSetBits() const
	{
		EnsurePartialSlackBitsCleared();
		return FVoxelBitArrayHelpers::CountSetBits(GetWordData(), NumWords());
	}
	FORCEINLINE SizeType CountSetBits(SizeType Count) const
	{
		checkVoxelSlow(Count <= Num());
		return FVoxelBitArrayHelpers::CountSetBits_UpperBound(GetWordData(), Count);
	}
	template<typename LambdaType>
	FORCEINLINE void ForAllSetBits(LambdaType Lambda) const
	{
		return FVoxelBitArrayHelpers::ForAllSetBits(GetWordData(), NumWords(), Num(), Lambda);
	}

public:
	FORCEINLINE int64 GetAllocatedSize() const
	{
		return FVoxelUtilities::DivideCeil_Positive(MaxBits, NumBitsPerWord) * sizeof(uint32);
	}
	FORCEINLINE SizeType Num() const
	{
		return NumBits;
	}
	FORCEINLINE SizeType NumWords() const
	{
		return FVoxelUtilities::DivideCeil_Positive(NumBits, NumBitsPerWord);
	}

public:
	FORCEINLINE int32 AddUninitialized(int32 NumBitsToAdd = 1)
	{
		checkVoxelSlow(NumBitsToAdd >= 0);

		const int32 OldNumBits = NumBits;

		const int32 OldLastWordIndex = NumBits == 0 ? -1 : (NumBits - 1) / NumBitsPerWord;
		const int32 NewLastWordIndex = (NumBits + NumBitsToAdd - 1) / NumBitsPerWord;
		if (NewLastWordIndex == OldLastWordIndex)
		{
			NumBits += NumBitsToAdd;
			EnsurePartialSlackBitsCleared();
			return OldNumBits;
		}

		this->Reserve(NumBits + NumBitsToAdd);

		NumBits += NumBitsToAdd;
		ClearPartialSlackBits();

		return OldNumBits;
	}
	FORCEINLINE int32 Add(const bool bValue)
	{
		const int32 Index = AddUninitialized(1);
		checkVoxelSlow(IsValidIndex(Index));
		FVoxelBitArrayHelpers::Set(GetWordData(), Index, bValue);
		return Index;
	}

	FORCEINLINE bool IsValidIndex(SizeType Index) const
	{
		return 0 <= Index && Index < NumBits;
	}

	FORCEINLINE FVoxelBitReference operator[](SizeType Index)
	{
		checkVoxelSlow(IsValidIndex(Index));
		return FVoxelBitReference(this->GetWord(Index / NumBitsPerWord), 1u << (Index % NumBitsPerWord));
	}
	FORCEINLINE FVoxelConstBitReference operator[](SizeType Index) const
	{
		checkVoxelSlow(IsValidIndex(Index));
		return FVoxelConstBitReference(this->GetWord(Index / NumBitsPerWord), 1u << (Index % NumBitsPerWord));
	}

	FORCEINLINE void AtomicallySet(SizeType Index, bool bValue)
	{
		uint32& Word = this->GetWord(Index / NumBitsPerWord);
		const uint32 Mask = 1u << (Index % NumBitsPerWord);

		if (bValue)
		{
			FPlatformAtomics::InterlockedOr(ReinterpretCastPtr<int32>(&Word), Mask);
		}
		else
		{
			FPlatformAtomics::InterlockedAnd(ReinterpretCastPtr<int32>(&Word), ~Mask);
		}
	}

public:
	FORCEINLINE uint32* GetWordData()
	{
		return static_cast<uint32*>(AllocatorInstance.GetAllocation());
	}
	FORCEINLINE const uint32* GetWordData() const
	{
		return static_cast<uint32*>(AllocatorInstance.GetAllocation());
	}

	FORCEINLINE TVoxelArrayView<uint32, SizeType> GetWordView()
	{
		return { GetWordData(), NumWords() };
	}
	FORCEINLINE TConstVoxelArrayView<uint32, SizeType> GetWordView() const
	{
		return { GetWordData(), NumWords() };
	}

	FORCEINLINE uint32& GetWord(SizeType Index)
	{
		checkVoxelSlow(0 <= Index && Index < NumWords());
		return GetWordData()[Index];
	}
	FORCEINLINE const uint32& GetWord(SizeType Index) const
	{
		checkVoxelSlow(0 <= Index && Index < NumWords());
		return GetWordData()[Index];
	}

private:
	using AllocatorType = typename Allocator::template ForElementType<uint32>;

	AllocatorType AllocatorInstance;
	SizeType NumBits = 0;
	SizeType MaxBits = 0;

	FORCENOINLINE void SetMaxBits(const SizeType NewMaxBits)
	{
		if (MaxBits == NewMaxBits)
		{
			return;
		}

		const SizeType PreviousMaxWords = FVoxelUtilities::DivideCeil_Positive(MaxBits, NumBitsPerWord);
		const SizeType NewMaxWords = FVoxelUtilities::DivideCeil_Positive(NewMaxBits, NumBitsPerWord);

		AllocatorInstance.ResizeAllocation(PreviousMaxWords, NewMaxWords, sizeof(uint32));

		MaxBits = NewMaxBits;
	}
	FORCEINLINE void ClearPartialSlackBits()
	{
		// TBitArray has a contract about bits outside of the active range - the bits in the final word past NumBits are guaranteed to be 0
		// This prevents easy-to-make determinism errors from users of TBitArray that do not carefully mask the final word.
		// It also allows us optimize some operations which would otherwise require us to mask the last word.

		const int32 UsedBits = NumBits % NumBitsPerWord;
		if (UsedBits == 0)
		{
			return;
		}

		const int32 LastWordIndex = NumBits / NumBitsPerWord;
		const uint32 SlackMask = 0xFFFFFFFF >> (NumBitsPerWord - UsedBits);

		GetWord(LastWordIndex) &= SlackMask;
	}
	FORCEINLINE void EnsurePartialSlackBitsCleared() const
	{
#if VOXEL_DEBUG
		const int32 UsedBits = NumBits % NumBitsPerWord;
		if (UsedBits == 0)
		{
			return;
		}

		const int32 LastWordIndex = NumBits / NumBitsPerWord;
		const uint32 SlackMask = 0xFFFFFFFF >> (NumBitsPerWord - UsedBits);

		ensure((GetWord(LastWordIndex) & ~SlackMask) == 0);
#endif
	}
};

template<typename Allocator>
uint32* RESTRICT GetData(TVoxelBitArray<Allocator>& Array)
{
	return Array.GetWordData();
}
template<typename Allocator>
const uint32* RESTRICT GetData(const TVoxelBitArray<Allocator>& Array)
{
	return Array.GetWordData();
}

// Can have different meanings
//template<typename Allocator>
//auto GetNum(const TVoxelBitArray<Allocator>& Array)
//{
//	return Array.Num();
//}

using FVoxelBitArray32 = TVoxelBitArray<FVoxelAllocator>;
using FVoxelBitArray64 = TVoxelBitArray<FVoxelAllocator64>;

struct FVoxelBitArrayVisualizerEntry
{
	uint32 Bit0 : 1;
	uint32 Bit1 : 1;
	uint32 Bit2 : 1;
	uint32 Bit3 : 1;
	uint32 Bit4 : 1;
	uint32 Bit5 : 1;
	uint32 Bit6 : 1;
	uint32 Bit7 : 1;
	uint32 Bit8 : 1;
	uint32 Bit9 : 1;
	uint32 Bit10 : 1;
	uint32 Bit11 : 1;
	uint32 Bit12 : 1;
	uint32 Bit13 : 1;
	uint32 Bit14 : 1;
	uint32 Bit15 : 1;
	uint32 Bit16 : 1;
	uint32 Bit17 : 1;
	uint32 Bit18 : 1;
	uint32 Bit19 : 1;
	uint32 Bit20 : 1;
	uint32 Bit21 : 1;
	uint32 Bit22 : 1;
	uint32 Bit23 : 1;
	uint32 Bit24 : 1;
	uint32 Bit25 : 1;
	uint32 Bit26 : 1;
	uint32 Bit27 : 1;
	uint32 Bit28 : 1;
	uint32 Bit29 : 1;
	uint32 Bit30 : 1;
	uint32 Bit31 : 1;
};