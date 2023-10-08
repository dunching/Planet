// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelTask.h"

extern VOXELGRAPHCORE_API bool GVoxelCheckNaNs;

template<typename>
class TVoxelBufferStorage;

struct FVoxelBufferDefinitions
{
	static constexpr int32 MaxISPCWidth = 8;
	static constexpr int32 MaxTypeSize = sizeof(uint64);
	static constexpr int32 Alignment = MaxISPCWidth * sizeof(uint32);
	static constexpr int32 NumPerChunkLog2 = 12;
	static constexpr int32 NumPerChunk = 1 << NumPerChunkLog2;

	FORCEINLINE static int32 GetChunkIndex(const int32 Index)
	{
		return FVoxelUtilities::GetChunkIndex<NumPerChunk>(Index);
	}
	FORCEINLINE static int32 GetChunkOffset(const int32 Index)
	{
		return FVoxelUtilities::GetChunkOffset<NumPerChunk>(Index);
	}
};
checkStatic(FVoxelBufferDefinitions::NumPerChunk * sizeof(float) == GVoxelDefaultAllocationSize);

struct FVoxelBufferIterator : FVoxelBufferDefinitions
{
public:
	int32 TotalNum = 0;
	int32 ChunkIndex = 0;
	int32 ChunkOffset = 0;
	int32 NumToProcess = 0;

	FORCEINLINE void Initialize(const int32 InTotalNum, const int32 Index)
	{
		TotalNum = InTotalNum;
		ChunkIndex = GetChunkIndex(Index);
		ChunkOffset = GetChunkOffset(Index);
		UpdateNumToProcess();
		checkVoxelSlow(IsValid());
	}
	FORCEINLINE bool IsValid() const
	{
		return
			ChunkOffset + NumToProcess <= NumPerChunk &&
			ChunkIndex * NumPerChunk + ChunkOffset + NumToProcess <= TotalNum;
	}

public:
	FORCEINLINE const FVoxelBufferIterator& operator*() const
	{
		return *this;
	}
	FORCEINLINE void operator++()
	{
		ChunkIndex++;
		ChunkOffset = 0;
		UpdateNumToProcess();
	}
	FORCEINLINE operator bool() const
	{
		return NumToProcess > 0;
	}
	FORCEINLINE bool operator!=(decltype(nullptr)) const
	{
		return NumToProcess > 0;
	}

public:
	FORCEINLINE int32 GetIndex() const
	{
		return ChunkIndex * NumPerChunk + ChunkOffset;
	}
	FORCEINLINE int32 Num() const
	{
		checkVoxelSlow(IsValid());
		return NumToProcess;
	}
	FORCEINLINE int32 AlignedNum() const
	{
		checkVoxelSlow(IsValid());
		checkStatic(Align(NumPerChunk, MaxISPCWidth) == NumPerChunk);
		return Align(NumToProcess, MaxISPCWidth);
	}

public:
	FORCEINLINE FVoxelBufferIterator GetHalfIterator() const
	{
		checkVoxelSlow(IsValid());

		checkVoxelSlow(TotalNum % 2 == 0);
		const int32 Index = GetIndex();

		checkVoxelSlow(Index % 2 == 0);
		const int32 HalfIndex = Index / 2;

		FVoxelBufferIterator HalfIterator;
		HalfIterator.Initialize(TotalNum / 2, HalfIndex);
		return HalfIterator;
	}
	FORCEINLINE void UpdateNumToProcess()
	{
		NumToProcess = FMath::Min(NumPerChunk - ChunkOffset, TotalNum - GetIndex());
	}
};

struct FVoxelBufferIteratorImpl : FVoxelBufferDefinitions
{
	const int32 Num;

	FORCEINLINE explicit FVoxelBufferIteratorImpl(const int32 Num)
		: Num(Num)
	{
	}

	FORCEINLINE FVoxelBufferIterator begin() const
	{
		FVoxelBufferIterator Iterator;
		Iterator.Initialize(Num, 0);
		return Iterator;
	}
	FORCEINLINE decltype(nullptr) end() const
	{
		return {};
	}
};

FORCEINLINE FVoxelBufferIteratorImpl MakeVoxelBufferIterator(const int32 Num)
{
	checkVoxelSlow(Num >= 0);
	return FVoxelBufferIteratorImpl(Num);
}

template<typename LambdaType>
FORCEINLINE void ForeachVoxelBufferChunk(const int32 Num, LambdaType&& Lambda)
{
	if (ShouldRunVoxelTaskInParallel())
	{
		constexpr int32 NumPerChunk = FVoxelBufferDefinitions::NumPerChunk;

		const int32 NumChunks = FVoxelUtilities::DivideCeil_Positive(Num, NumPerChunk);
		ParallelFor(NumChunks, [&](const int32 ChunkIndex)
		{
			FVoxelBufferIterator Iterator;
			Iterator.Initialize(Num, ChunkIndex * NumPerChunk);
			Lambda(Iterator);
		});
	}
	else
	{
		for (const FVoxelBufferIterator& Iterator : MakeVoxelBufferIterator(Num))
		{
			Lambda(Iterator);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<int32 Count>
class TVoxelBufferMultiIterator : public FVoxelBufferDefinitions
{
public:
	const TVoxelStaticArray<int32, Count> Nums;

	template<typename... ArgTypes>
	FORCEINLINE explicit TVoxelBufferMultiIterator(ArgTypes... Args)
		: Nums(Args...)
	{
		UpdateStep();
	}

	template<int32 Index>
	FORCEINLINE void SetIndex(const int32 InIndex)
	{
		checkStatic(0 <= Index && Index < Count);
		checkVoxelSlow(0 <= InIndex && InIndex < Nums[Index]);

		ChunkIndices[Index] = GetChunkIndex(InIndex);
		ChunkOffsets[Index] = GetChunkOffset(InIndex);

		UpdateStep();
	}

	FORCEINLINE void operator++()
	{
		for (int32 Index = 0; Index < Count; Index++)
		{
			ChunkOffsets[Index] += Step;
			checkVoxelSlow(ChunkOffsets[Index] <= NumPerChunk);

			if (ChunkOffsets[Index] == NumPerChunk)
			{
				ChunkOffsets[Index] = 0;
				ChunkIndices[Index]++;
			}
		}

		UpdateStep();
	}
	FORCEINLINE operator bool() const
	{
		for (int32 Index = 0; Index < Count; Index++)
		{
			const int32 CurrentIndex = ChunkIndices[Index] * NumPerChunk + ChunkOffsets[Index];
			if (CurrentIndex < Nums[Index])
			{
				continue;
			}
			for (int32 OtherIndex = 0; OtherIndex < Count; OtherIndex++)
			{
				checkVoxelSlow(ChunkIndices[OtherIndex] * NumPerChunk + ChunkOffsets[OtherIndex] == Nums[OtherIndex]);
			}
			return false;
		}

		return true;
	}

	template<int32 Index>
	FORCEINLINE FVoxelBufferIterator Get() const
	{
		checkStatic(0 <= Index && Index < Count);

		FVoxelBufferIterator Iterator;
		Iterator.TotalNum = Nums[Index];
		Iterator.ChunkIndex = ChunkIndices[Index];
		Iterator.ChunkOffset = ChunkOffsets[Index];
		Iterator.NumToProcess = Step;
		return Iterator;
	}

private:
	int32 Step = 0;
	TVoxelStaticArray<int32, Count> ChunkIndices{ ForceInit };
	TVoxelStaticArray<int32, Count> ChunkOffsets{ ForceInit };

	FORCEINLINE void UpdateStep()
	{
		Step = NumPerChunk;

		for (int32 Index = 0; Index < Count; Index++)
		{
			Step = FMath::Min3(
				Step,
				// Left in chunk
				NumPerChunk - ChunkOffsets[Index],
				// Left in array
				Nums[Index] - ChunkIndices[Index] * NumPerChunk - ChunkOffsets[Index]);
		}
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_MEMORY_STAT(VOXELGRAPHCORE_API, STAT_VoxelBufferStorage, "Buffer Storage");

class VOXELGRAPHCORE_API FVoxelBufferStorage : public FVoxelBufferDefinitions
{
public:
	FORCEINLINE explicit FVoxelBufferStorage(const int32 TypeSize)
		: TypeSize(TypeSize)
	{
		checkVoxelSlow(TypeSize <= MaxTypeSize);
	}
	FORCEINLINE FVoxelBufferStorage(FVoxelBufferStorage&& Other)
		: TypeSize(Other.TypeSize)
		, ArrayNum(Other.ArrayNum)
		, bCanGrow(Other.bCanGrow)
		, Chunks(MoveTemp(Other.Chunks))
	{
		Other.ArrayNum = 0;
		Other.bCanGrow = true;
	}
	FVoxelBufferStorage(const FVoxelBufferStorage&) = delete;

	FORCEINLINE ~FVoxelBufferStorage()
	{
		Empty();
	}

	FORCEINLINE FVoxelBufferStorage& operator=(FVoxelBufferStorage&& Other)
	{
		checkVoxelSlow(TypeSize == Other.TypeSize);

		if (Chunks.Num() > 0)
		{
			Empty();
		}
		checkVoxelSlow(Chunks.Num() == 0);

		ArrayNum = Other.ArrayNum;
		bCanGrow = Other.bCanGrow;
		Chunks = MoveTemp(Other.Chunks);

		Other.ArrayNum = 0;
		Other.bCanGrow = true;

		return *this;
	}
	FVoxelBufferStorage& operator=(const FVoxelBufferStorage&) = delete;

	template<typename Type>
	FORCEINLINE TVoxelBufferStorage<Type>& As()
	{
		checkVoxelSlow(sizeof(Type) == TypeSize);
		return static_cast<TVoxelBufferStorage<Type>&>(*this);
	}
	template<typename Type>
	FORCEINLINE const TVoxelBufferStorage<Type>& As() const
	{
		checkVoxelSlow(sizeof(Type) == TypeSize);
		return static_cast<const TVoxelBufferStorage<Type>&>(*this);
	}

public:
	// Set bAllowGrowth default to true for better allocation reuse
	void Allocate(const int32 Num, bool bAllowGrowth = true);
	void Empty();
	void Reserve(const int32 Num);
	void Shrink();
	int64 GetAllocatedSize() const;
	bool TryReduceIntoConstant();
	void FixupAlignmentPaddingData() const;
	void CheckSlow(const FVoxelPinType& Type) const;
	TSharedRef<FVoxelBufferStorage> Clone() const;
	int32 AddUninitialized(int32 NumToAdd);
	void AddZeroed(int32 NumToAdd);
	void Append(const FVoxelBufferStorage& BufferStorage, int32 BufferNum);
	void CopyTo(const TVoxelArrayView<uint8> OtherData) const;

	FORCEINLINE int32 GetTypeSize() const
	{
		return TypeSize;
	}
	FORCEINLINE int32 Num() const
	{
		return ArrayNum;
	}
	FORCEINLINE bool IsConstant() const
	{
		return Num() == 1;
	}
	FORCEINLINE int32 AddUninitialized()
	{
		checkVoxelSlow(bCanGrow);
		if (ArrayNum % NumPerChunk == 0)
		{
			AddUninitialized_Allocate();
		}
		return ArrayNum++;
	}
	FORCEINLINE bool IsValidIndex(const int32 Index) const
	{
		return 0 <= Index && (IsConstant() || Index < ArrayNum);
	}

public:
	void CheckIterator(const FVoxelBufferIterator& Iterator) const;

	uint8* GetByteData(const FVoxelBufferIterator& Iterator);
	const uint8* GetByteData(const FVoxelBufferIterator& Iterator) const;

	TVoxelArrayView<uint8> GetByteRawView_NotConstant(const FVoxelBufferIterator& Iterator);
	TConstVoxelArrayView<uint8> GetByteRawView_NotConstant(const FVoxelBufferIterator& Iterator) const;

protected:
	int32 TypeSize = 0;
	int32 ArrayNum = 0;
	bool bCanGrow = true;
	TVoxelArray<void*, TVoxelInlineAllocator<2>> Chunks;

private:
	void AddUninitialized_Allocate();
	void* AllocateChunk(int32 Num = NumPerChunk) const;

	VOXEL_ALLOCATED_SIZE_TRACKER_CUSTOM(STAT_VoxelBufferStorage, AllocatedSizeTracker);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename Type>
class TVoxelBufferStorageBase : public FVoxelBufferStorage
{
public:
	checkStatic(TIsTriviallyDestructible<Type>::Value);
	checkStatic(sizeof(Type) <= MaxTypeSize);

	// Update FVoxelSwitchTerminalTypeSize if this fails
	checkStatic(
		sizeof(Type) == 1 ||
		sizeof(Type) == 2 ||
		sizeof(Type) == 4 ||
		sizeof(Type) == 8);

	FORCEINLINE TVoxelBufferStorageBase()
		: FVoxelBufferStorage(sizeof(Type))
	{
	}

public:
	FORCEINLINE auto GetData(const FVoxelBufferIterator& Iterator) -> decltype(auto)
	{
		if constexpr (std::is_same_v<Type, bool>)
		{
			// bool should be passed as uint8 to ISPC when writing, as we need to manually set them to 0 or 1
			return GetByteData(Iterator);
		}
		else
		{
			return reinterpret_cast<Type*>(GetByteData(Iterator));
		}
	}
	FORCEINLINE const Type* GetData(const FVoxelBufferIterator& Iterator) const
	{
		return reinterpret_cast<const Type*>(GetByteData(Iterator));
	}

	FORCEINLINE TVoxelArrayView<Type> GetRawView_NotConstant(const FVoxelBufferIterator& Iterator)
	{
		return ReinterpretCastVoxelArrayView<Type>(GetByteRawView_NotConstant(Iterator));
	}
	FORCEINLINE TConstVoxelArrayView<Type> GetRawView_NotConstant(const FVoxelBufferIterator& Iterator) const
	{
		return ReinterpretCastVoxelArrayView<Type>(GetByteRawView_NotConstant(Iterator));
	}

private:
	using FVoxelBufferStorage::GetByteData;
	using FVoxelBufferStorage::GetByteRawView_NotConstant;

public:
	FORCEINLINE void SetConstant(const Type Constant)
	{
		Allocate(1, false);
		(*this)[0] = Constant;
	}
	FORCEINLINE const Type& GetConstant() const
	{
		checkVoxelSlow(IsConstant());
		return LoadFast(0);
	}

public:
	FORCEINLINE int32 Add(const Type Value)
	{
		const int32 Index = AddUninitialized();
		LoadFast(Index) = Value;
		return Index;
	}
	FORCENOINLINE void BulkAdd(const Type Value, const int32 NumToAdd)
	{
		VOXEL_FUNCTION_COUNTER_NUM(NumToAdd, 1024);

		if (NumToAdd == 0)
		{
			return;
		}

		const int32 Index = AddUninitialized(NumToAdd);

		FVoxelBufferIterator Iterator;
		Iterator.Initialize(Num(), Index);
		for (; Iterator; ++Iterator)
		{
			FVoxelUtilities::SetAll(GetRawView_NotConstant(Iterator), Value);
		}
	}

	FORCEINLINE void CopyTo(const TVoxelArrayView<Type> OtherData) const
	{
		FVoxelBufferStorage::CopyTo(MakeByteVoxelArrayView(OtherData));
	}

	FORCEINLINE Type& operator[](int32 Index)
	{
		checkVoxelSlow(IsValidIndex(Index));
		checkVoxelSlow(TypeSize == sizeof(Type));

		const int32 Mask = ArrayNum != 1;
		Index *= Mask;

		Type* RESTRICT Chunk = static_cast<Type*>(Chunks[GetChunkIndex(Index)]);
		checkVoxelSlow(Chunk);
		return Chunk[GetChunkOffset(Index)];
	}
	FORCEINLINE const Type& operator[](const int32 Index) const
	{
		return ConstCast(this)->operator[](Index);
	}

	FORCEINLINE Type& LoadFast(const int32 Index)
	{
		checkVoxelSlow(0 <= Index && Index < ArrayNum);
		checkVoxelSlow(TypeSize == sizeof(Type));

		Type* RESTRICT Chunk = static_cast<Type*>(Chunks[GetChunkIndex(Index)]);
		checkVoxelSlow(Chunk);
		return Chunk[GetChunkOffset(Index)];
	}
	FORCEINLINE const Type& LoadFast(const int32 Index) const
	{
		return ConstCast(this)->LoadFast(Index);
	}

public:
	template<typename InType>
	struct TIterator
	{
		Type* Value = nullptr;
		Type* ChunkEndValue = nullptr;
		void* const* ChunkIterator = nullptr;

		FORCEINLINE InType& operator*() const
		{
			return *Value;
		}
		FORCEINLINE void operator++()
		{
			checkVoxelSlow(Value);
			++Value;

			if (Value != ChunkEndValue)
			{
				return;
			}

			++ChunkIterator;

			// If we are at the last chunk, Value will be nullptr
			Value = static_cast<Type*>(*ChunkIterator);
			ChunkEndValue = static_cast<Type*>(*ChunkIterator) + NumPerChunk;
		}
		FORCEINLINE bool operator!=(const Type* End)
		{
			return Value != End;
		}
	};

	FORCEINLINE TIterator<Type> begin()
	{
		if (ArrayNum == 0)
		{
			return {};
		}

		// Chunks should always have a null chunk at the end
		checkVoxelSlow(!Chunks.Last());

		return
		{
			static_cast<Type*>(Chunks[0]),
			static_cast<Type*>(Chunks[0]) + NumPerChunk,
			Chunks.GetData(),
		};
	}
	FORCEINLINE TIterator<const Type> begin() const
	{
		return ReinterpretCastRef<TIterator<const Type>>(ConstCast(this)->begin());
	}

	FORCEINLINE const Type* end() const
	{
		if (ArrayNum == 0)
		{
			return nullptr;
		}

		const int32 ChunkIndex = GetChunkIndex(ArrayNum);
		if (ChunkIndex >= Chunks.Num() - 1)
		{
			ensureVoxelSlow(ChunkIndex == Chunks.Num() - 1);
			// Null chunk at the end will handle termination
			return nullptr;
		}

		checkVoxelSlow(Chunks[ChunkIndex]);
		return &static_cast<const Type*>(Chunks[ChunkIndex])[GetChunkOffset(ArrayNum)];
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename Type>
class TVoxelBufferStorage : public TVoxelBufferStorageBase<Type>
{
public:
	using TVoxelBufferStorageBase<Type>::TVoxelBufferStorageBase;

	FORCEINLINE TSharedRef<TVoxelBufferStorage> Clone() const
	{
		return StaticCastSharedRef<TVoxelBufferStorage>(FVoxelBufferStorage::Clone());
	}
};

template<>
class TVoxelBufferStorage<float> : public TVoxelBufferStorageBase<float>
{
public:
	using TVoxelBufferStorageBase::TVoxelBufferStorageBase;

	FORCEINLINE TSharedRef<TVoxelBufferStorage> Clone() const
	{
		return StaticCastSharedRef<TVoxelBufferStorage>(FVoxelBufferStorage::Clone());
	}

	// Turn -0 into +0
	VOXELGRAPHCORE_API void FixupSignBit();
	VOXELGRAPHCORE_API FFloatInterval GetMinMaxSafe() const;
};