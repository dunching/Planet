// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"

template<typename InElementType, typename InAllocator = FVoxelAllocator>
class TVoxelArray : public TArray<InElementType, InAllocator>
{
public:
	using Super = TArray<InElementType, InAllocator>;
	using typename Super::SizeType;
	using typename Super::ElementType;
	using typename Super::AllocatorType;
	using Super::GetData;
	using Super::CheckInvariants;
	using Super::Shrink;
	using Super::ArrayNum;
	using Super::ArrayMax;
	using Super::AllocatorInstance;
	using Super::Num;
	using Super::Super;

	using USizeType = typename TMakeUnsigned<SizeType>::Type;

#if !defined(_MSC_VER) || _MSC_VER > 1929 || PLATFORM_COMPILER_CLANG // Copy constructors are inherited on MSVC, but implementing them manually deletes all the other inherited constructors
	template<typename OtherElementType, typename OtherAllocator>
	FORCEINLINE explicit TVoxelArray(const TArray<OtherElementType, OtherAllocator>& Other)
		: Super(Other)
	{
	}
#endif

	template<typename OtherAllocator>
	FORCEINLINE TVoxelArray& operator=(const TArray<ElementType, OtherAllocator>& Other)
	{
		Super::operator=(Other);
		return *this;
	}

public:
	FORCEINLINE void RangeCheck(SizeType Index) const
	{
		CheckInvariants();

		// Template property, branch will be optimized out
		if (AllocatorType::RequireRangeCheck)
		{
			checkfVoxelSlow((Index >= 0) & (Index < ArrayNum), TEXT("Array index out of bounds: %i from an array of size %i"), Index, ArrayNum); // & for one branch
		}
	}
	FORCEINLINE void CheckAddress(const ElementType* Addr) const
	{
		checkfVoxelSlow(Addr < GetData() || Addr >= (GetData() + ArrayMax), TEXT("Attempting to use a container element (%p) which already comes from the container being modified (%p, ArrayMax: %d, ArrayNum: %d, SizeofElement: %d)!"), Addr, GetData(), ArrayMax, ArrayNum, sizeof(ElementType));
	}

public:
	// Cannot be used to change the allocation!
	FORCEINLINE const TArray<InElementType>& ToConstArray() const
	{
		return ReinterpretCastRef<TArray<InElementType>>(*this);
	}

	FORCEINLINE ElementType Pop(const bool bAllowShrinking = true)
	{
		RangeCheck(0);
		ElementType Result = MoveTempIfPossible(GetData()[ArrayNum - 1]);

		DestructItem(GetData() + ArrayNum - 1);
		ArrayNum--;

		if (bAllowShrinking)
		{
			Shrink();
		}

		return Result;
	}

	FORCEINLINE SizeType Add_NoGrow(const ElementType& Item)
	{
		CheckAddress(&Item);
		checkVoxelSlow(ArrayNum < ArrayMax);

		const SizeType Index = ArrayNum++;

		ElementType* Ptr = GetData() + Index;
		new (Ptr) ElementType(Item);
		return Index;
	}

	FORCEINLINE SizeType Add(ElementType&& Item)
	{
		CheckAddress(&Item);
		return this->Emplace(MoveTempIfPossible(Item));
	}
	FORCEINLINE SizeType Add(const ElementType& Item)
	{
		CheckAddress(&Item);
		return this->Emplace(Item);
	}
	FORCEINLINE ElementType& Add_GetRef(ElementType&& Item)
	{
		CheckAddress(&Item);
		return this->Emplace_GetRef(MoveTempIfPossible(Item));
	}
	FORCEINLINE ElementType& Add_GetRef(const ElementType& Item)
	{
		CheckAddress(&Item);
		return this->Emplace_GetRef(Item);
	}

	template<typename... ArgsType>
	FORCEINLINE SizeType Emplace(ArgsType&&... Args)
	{
		const SizeType Index = AddUninitialized();
		new (GetData() + Index) ElementType(Forward<ArgsType>(Args)...);
		return Index;
	}
	template<typename... ArgsType>
	FORCEINLINE ElementType& Emplace_GetRef(ArgsType&&... Args)
	{
		const SizeType Index = AddUninitialized();
		ElementType* Ptr = GetData() + Index;
		new (Ptr) ElementType(Forward<ArgsType>(Args)...);
		return *Ptr;
	}

	FORCEINLINE SizeType Find(const ElementType& Item) const
	{
		const ElementType* RESTRICT Start = GetData();
		const ElementType* RESTRICT End = Start + ArrayNum;
		for (const ElementType* RESTRICT Data = Start; Data != End; ++Data)
		{
			if (*Data == Item)
			{
				return static_cast<SizeType>(Data - Start);
			}
		}
		return INDEX_NONE;
	}
	template<typename ComparisonType>
	FORCEINLINE bool Contains(const ComparisonType& Item) const
	{
		const ElementType* RESTRICT Start = GetData();
		const ElementType* RESTRICT End = Start + ArrayNum;
		for (const ElementType* RESTRICT Data = Start; Data != End; ++Data)
		{
			if (*Data == Item)
			{
				return true;
			}
		}
		return false;
	}

	FORCEINLINE SizeType AddUnique(const ElementType& Item)
	{
		const SizeType Index = this->Find(Item);
		if (Index != -1)
		{
			return Index;
		}

		return this->Add(Item);
	}
	FORCEINLINE SizeType AddUninitialized(SizeType Count = 1)
	{
		CheckInvariants();
		checkVoxelSlow(Count >= 0);

		const USizeType OldNum = USizeType(ArrayNum);
		const USizeType NewNum = OldNum + USizeType(Count);
		ArrayNum = SizeType(NewNum);

		if (NewNum > USizeType(ArrayMax))
		{
			this->ResizeGrow(SizeType(OldNum));
		}
		return OldNum;
	}

private:
	FORCENOINLINE void ResizeGrow(SizeType OldNum)
	{
		checkVoxelSlow(OldNum < ArrayNum);

		ArrayMax = this->AllocatorCalculateSlackGrow(ArrayNum, ArrayMax);
		this->AllocatorResizeAllocation(OldNum, ArrayMax);
	}
	void AllocatorResizeAllocation(SizeType CurrentArrayNum, SizeType NewArrayMax)
	{
		VOXEL_FUNCTION_COUNTER_NUM(NewArrayMax, 1024);

		if constexpr (TAllocatorTraits<AllocatorType>::SupportsElementAlignment)
		{
			AllocatorInstance.ResizeAllocation(CurrentArrayNum, NewArrayMax, sizeof(ElementType), alignof(ElementType));
		}
		else
		{
			AllocatorInstance.ResizeAllocation(CurrentArrayNum, NewArrayMax, sizeof(ElementType));
		}
	}
	SizeType AllocatorCalculateSlackGrow(SizeType CurrentArrayNum, SizeType NewArrayMax)
	{
		if constexpr (TAllocatorTraits<AllocatorType>::SupportsElementAlignment)
		{
			return AllocatorInstance.CalculateSlackGrow(CurrentArrayNum, NewArrayMax, sizeof(ElementType), alignof(ElementType));
		}
		else
		{
			return AllocatorInstance.CalculateSlackGrow(CurrentArrayNum, NewArrayMax, sizeof(ElementType));
		}
	}

public:
	FORCEINLINE void SwapMemory(SizeType FirstIndexToSwap, SizeType SecondIndexToSwap)
	{
		// FMemory::Memswap is not inlined

		checkStatic(TIsTriviallyDestructible<ElementType>::Value);

		ElementType& A = (*this)[FirstIndexToSwap];
		ElementType& B = (*this)[SecondIndexToSwap];

		TTypeCompatibleBytes<ElementType> Temp;
		FMemory::Memcpy(&Temp, &A, sizeof(ElementType));
		FMemory::Memcpy(&A, &B, sizeof(ElementType));
		FMemory::Memcpy(&B, &Temp, sizeof(ElementType));
	}
	FORCEINLINE void Swap(SizeType FirstIndexToSwap, SizeType SecondIndexToSwap)
	{
		checkVoxelSlow((FirstIndexToSwap >= 0) && (SecondIndexToSwap >= 0));
		checkVoxelSlow((ArrayNum > FirstIndexToSwap) && (ArrayNum > SecondIndexToSwap));
		this->SwapMemory(FirstIndexToSwap, SecondIndexToSwap);
	}

public:
	FORCEINLINE ElementType& operator[](SizeType Index)
	{
		RangeCheck(Index);
		return GetData()[Index];
	}

	FORCEINLINE const ElementType& operator[](SizeType Index) const
	{
		RangeCheck(Index);
		return GetData()[Index];
	}

public:
#if TARRAY_RANGED_FOR_CHECKS && VOXEL_DEBUG
	typedef TCheckedPointerIterator<      ElementType, SizeType> RangedForIteratorType;
	typedef TCheckedPointerIterator<const ElementType, SizeType> RangedForConstIteratorType;
#else
	typedef       ElementType* RangedForIteratorType;
	typedef const ElementType* RangedForConstIteratorType;
#endif

#if TARRAY_RANGED_FOR_CHECKS && VOXEL_DEBUG
	FORCEINLINE RangedForIteratorType      begin() { return RangedForIteratorType(ArrayNum, GetData()); }
	FORCEINLINE RangedForConstIteratorType begin() const { return RangedForConstIteratorType(ArrayNum, GetData()); }
	FORCEINLINE RangedForIteratorType      end() { return RangedForIteratorType(ArrayNum, GetData() + Num()); }
	FORCEINLINE RangedForConstIteratorType end() const { return RangedForConstIteratorType(ArrayNum, GetData() + Num()); }
#else
	FORCEINLINE RangedForIteratorType      begin() { return GetData(); }
	FORCEINLINE RangedForConstIteratorType begin() const { return GetData(); }
	FORCEINLINE RangedForIteratorType      end() { return GetData() + Num(); }
	FORCEINLINE RangedForConstIteratorType end() const { return GetData() + Num(); }
#endif
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename To, typename ElementType, typename Allocator>
FORCEINLINE TVoxelArray<To, Allocator>& ReinterpretCastVoxelArray(TVoxelArray<ElementType, Allocator>& Array)
{
	static_assert(sizeof(To) == sizeof(ElementType), "");
	return reinterpret_cast<TVoxelArray<To, Allocator>&>(Array);
}

template<typename To, typename ElementType, typename Allocator>
FORCEINLINE const TVoxelArray<To, Allocator>& ReinterpretCastVoxelArray(const TVoxelArray<ElementType, Allocator>& Array)
{
	static_assert(sizeof(To) == sizeof(ElementType), "");
	return reinterpret_cast<const TVoxelArray<To, Allocator>&>(Array);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename ToType, typename ToAllocator, typename FromType, typename Allocator, typename = typename TEnableIf<sizeof(FromType) != sizeof(ToType)>::Type>
TVoxelArray<ToType, ToAllocator> ReinterpretCastVoxelArray_Copy(const TVoxelArray<FromType, Allocator>& Array)
{
	const int64 NumBytes = Array.Num() * sizeof(FromType);
	check(NumBytes % sizeof(ToType) == 0);
	return TVoxelArray<ToType, Allocator>(reinterpret_cast<const ToType*>(Array.GetData()), NumBytes / sizeof(ToType));
}
template<typename ToType, typename FromType, typename Allocator, typename = typename TEnableIf<sizeof(FromType) != sizeof(ToType)>::Type>
TVoxelArray<ToType, Allocator> ReinterpretCastVoxelArray_Copy(const TVoxelArray<FromType, Allocator>& Array)
{
	return ReinterpretCastVoxelArray_Copy<ToType, Allocator>(Array);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename InElementType, typename Allocator>
struct TIsZeroConstructType<TVoxelArray<InElementType, Allocator>> : TIsZeroConstructType<TArray<InElementType, Allocator>>
{
};

template<typename T, typename Allocator>
struct TIsContiguousContainer<TVoxelArray<T, Allocator>> : TIsContiguousContainer<TArray<T, Allocator>>
{
};

template<typename InElementType, typename Allocator> struct TIsTArray<               TVoxelArray<InElementType, Allocator>> { enum { Value = true }; };
template<typename InElementType, typename Allocator> struct TIsTArray<const          TVoxelArray<InElementType, Allocator>> { enum { Value = true }; };
template<typename InElementType, typename Allocator> struct TIsTArray<      volatile TVoxelArray<InElementType, Allocator>> { enum { Value = true }; };
template<typename InElementType, typename Allocator> struct TIsTArray<const volatile TVoxelArray<InElementType, Allocator>> { enum { Value = true }; };

template<typename T>
using TVoxelArray64 = TVoxelArray<T, FVoxelAllocator64>;

// No runtime checks, but same allocator as TArray
template<typename T>
using TCompatibleVoxelArray = TVoxelArray<T, FDefaultAllocator>;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename ElementType>
FORCEINLINE TCompatibleVoxelArray<ElementType>& ToCompatibleVoxelArray(TArray<ElementType>& Array)
{
	return static_cast<TCompatibleVoxelArray<ElementType>&>(Array);
}
template<typename ElementType>
FORCEINLINE const TCompatibleVoxelArray<ElementType>& ToCompatibleVoxelArray(const TArray<ElementType>& Array)
{
	return static_cast<const TCompatibleVoxelArray<ElementType>&>(Array);
}
template<typename ElementType>
FORCEINLINE TCompatibleVoxelArray<ElementType>&& ToCompatibleVoxelArray(TArray<ElementType>&& Array)
{
	return static_cast<TCompatibleVoxelArray<ElementType>&>(MoveTemp(Array));
}