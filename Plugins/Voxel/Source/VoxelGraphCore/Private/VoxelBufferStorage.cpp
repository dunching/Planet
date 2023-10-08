// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelBufferStorage.h"
#include "Buffer/VoxelBaseBuffers.h"
#include "VoxelBufferImpl.ispc.generated.h"

DEFINE_VOXEL_MEMORY_STAT(STAT_VoxelBufferStorage);

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHCORE_API, bool, GVoxelCheckNaNs, false,
	"voxel.CheckNaNs",
	"");

#if !UE_BUILD_SHIPPING
VOXEL_RUN_ON_STARTUP_GAME(InitializeVoxelCheckNaNs)
{
	GVoxelCheckNaNs = FParse::Param(FCommandLine::Get(), TEXT("checkVoxelNaNs"));
}
#endif

void FVoxelBufferStorage::Allocate(const int32 Num, const bool bAllowGrowth)
{
	VOXEL_FUNCTION_COUNTER_NUM(Num, 8192);
	check(Num >= 0);
	check(TypeSize > 0);
	checkf(ArrayNum == 0, TEXT("Buffer already allocated"));
	check(bCanGrow);
	check(Chunks.Num() == 0);

	if (Num == 0)
	{
		return;
	}

	ArrayNum = Num;
	bCanGrow = bAllowGrowth;

	const int32 NumChunks = FMath::DivideAndRoundUp(Num, NumPerChunk);
	Chunks.Reserve(NumChunks + 1);

	const int32 NumInLastChunk = Align(Num, MaxISPCWidth) % NumPerChunk;
	for (int32 Index = 0; Index < NumChunks; Index++)
	{
		if (bAllowGrowth ||
			NumInLastChunk == 0 ||
			Index != NumChunks - 1)
		{
			Chunks.Add(AllocateChunk());
			continue;
		}

		Chunks.Add(AllocateChunk(NumInLastChunk));
	}
	Chunks.Add(nullptr);
	check(Chunks.Num() == NumChunks + 1);

	AllocatedSizeTracker = GetAllocatedSize();
}

void FVoxelBufferStorage::Empty()
{
	ArrayNum = 0;
	bCanGrow = true;

	if (Chunks.Num() > 0)
	{
		verify(Chunks.Pop(false) == nullptr);
		for (void* Chunk : Chunks)
		{
			check(Chunk);
			FVoxelMemory::Free(Chunk);
		}
		Chunks.Empty();
	}

	AllocatedSizeTracker = GetAllocatedSize();
}

void FVoxelBufferStorage::Reserve(const int32 Num)
{
	const int32 NumChunks = FMath::DivideAndRoundUp(Num, NumPerChunk);
	Chunks.Reserve(NumChunks);
}

void FVoxelBufferStorage::Shrink()
{
	VOXEL_FUNCTION_COUNTER_NUM(Num(), 1024);

	if (Chunks.Num() == 0)
	{
		return;
	}

	Chunks.Shrink();

	const int32 NumInLastChunk = Align(Num(), MaxISPCWidth) % NumPerChunk;
	if (NumInLastChunk == 0)
	{
		return;
	}

	if (!bCanGrow)
	{
		return;
	}
	bCanGrow = false;

	ensure(Chunks.Last() == nullptr);

	void* NewChunk = AllocateChunk(NumInLastChunk);
	void*& OldChunk = Chunks.Last(1);

	FMemory::Memcpy(NewChunk, OldChunk, TypeSize * NumInLastChunk);

	FVoxelMemory::Free(OldChunk);
	OldChunk = NewChunk;

	AllocatedSizeTracker = GetAllocatedSize();
}

int64 FVoxelBufferStorage::GetAllocatedSize() const
{
	int64 AllocatedSize = Chunks.GetAllocatedSize();
	if (!bCanGrow)
	{
		AllocatedSize += Align(Num(), MaxISPCWidth) * TypeSize;
	}
	else if (Chunks.Num() > 0)
	{
		// Last chunk is null for iteration
		ensure(!Chunks.Last());
		ensure(Chunks.Num() >= 2);
		AllocatedSize += (Chunks.Num() - 1) * NumPerChunk * TypeSize;
	}
	return AllocatedSize;
}

bool FVoxelBufferStorage::TryReduceIntoConstant()
{
	VOXEL_FUNCTION_COUNTER();

	if (Num() == 0)
	{
		return false;
	}
	if (IsConstant())
	{
		return true;
	}

	if (TypeSize == sizeof(uint8))
	{
		TVoxelBufferStorage<uint8>& This = static_cast<TVoxelBufferStorage<uint8>&>(*this);

		const uint8 Constant = This[0];
		for (const FVoxelBufferIterator& Iterator : MakeVoxelBufferIterator(Num()))
		{
			if (!FVoxelUtilities::AllEqual(Constant, This.GetRawView_NotConstant(Iterator)))
			{
				return false;
			}
		}

		This.Empty();
		This.SetConstant(Constant);
		return true;
	}
	else if (TypeSize == sizeof(uint16))
	{
		TVoxelBufferStorage<uint16>& This = static_cast<TVoxelBufferStorage<uint16>&>(*this);

		const uint16 Constant = This[0];
		for (const FVoxelBufferIterator& Iterator : MakeVoxelBufferIterator(Num()))
		{
			if (!FVoxelUtilities::AllEqual(Constant, This.GetRawView_NotConstant(Iterator)))
			{
				return false;
			}
		}

		This.Empty();
		This.SetConstant(Constant);
		return true;
	}
	else if (TypeSize == sizeof(uint32))
	{
		TVoxelBufferStorage<uint32>& This = static_cast<TVoxelBufferStorage<uint32>&>(*this);

		const uint32 Constant = This[0];
		for (const FVoxelBufferIterator& Iterator : MakeVoxelBufferIterator(Num()))
		{
			if (!FVoxelUtilities::AllEqual(Constant, This.GetRawView_NotConstant(Iterator)))
			{
				return false;
			}
		}

		This.Empty();
		This.SetConstant(Constant);
		return true;
	}
	else if (TypeSize == sizeof(uint64))
	{
		TVoxelBufferStorage<uint64>& This = static_cast<TVoxelBufferStorage<uint64>&>(*this);

		const uint64 Constant = This[0];
		for (const FVoxelBufferIterator& Iterator : MakeVoxelBufferIterator(Num()))
		{
			if (!FVoxelUtilities::AllEqual(Constant, This.GetRawView_NotConstant(Iterator)))
			{
				return false;
			}
		}

		This.Empty();
		This.SetConstant(Constant);
		return true;
	}
	else
	{
		ensure(false);
		return false;
	}
}

void FVoxelBufferStorage::FixupAlignmentPaddingData() const
{
	if (IsConstant())
	{
		uint8* Chunk = static_cast<uint8*>(Chunks[0]);
		check(Chunk);
		for (int32 Index = 1; Index < MaxISPCWidth; Index++)
		{
			FMemory::Memcpy(Chunk + TypeSize * Index, Chunk, TypeSize);
		}
		return;
	}

	const int32 AlignedNum = Align(Num(), MaxISPCWidth);
	if (AlignedNum == Num())
	{
		return;
	}

	const void* ReadChunk = Chunks[0];
	check(ReadChunk);

	check(GetChunkIndex(Num()) == GetChunkIndex(AlignedNum - 1));
	uint8* WriteChunk = static_cast<uint8*>(Chunks[GetChunkIndex(Num())]);
	check(WriteChunk);

	for (int32 Index = Num(); Index < AlignedNum; Index++)
	{
		FMemory::Memcpy(
			WriteChunk + GetChunkOffset(Index) * TypeSize,
			ReadChunk,
			TypeSize);
	}
}

void FVoxelBufferStorage::CheckSlow(const FVoxelPinType& Type) const
{
	if (!GVoxelCheckNaNs)
	{
		return;
	}

	VOXEL_FUNCTION_COUNTER();

	if (Type.Is<bool>())
	{
		for (const bool& Value : As<bool>())
		{
			ensure(
				ReinterpretCastRef<uint8>(Value) == 0 ||
				ReinterpretCastRef<uint8>(Value) == 1);
		}
	}
	else if (Type.Is<float>())
	{
		for (const float& Value : As<float>())
		{
			ensure(FMath::IsFinite(Value));
		}
	}
	else if (Type.Is<double>())
	{
		for (const double& Value : As<double>())
		{
			ensure(FMath::IsFinite(Value));
		}
	}
}

TSharedRef<FVoxelBufferStorage> FVoxelBufferStorage::Clone() const
{
	VOXEL_FUNCTION_COUNTER_NUM(Num(), 1024);

	const TSharedRef<FVoxelBufferStorage> Result = MakeVoxelShared<FVoxelBufferStorage>(TypeSize);

	if (Chunks.Num() == 0)
	{
		ensure(Num() == 0);
		return Result;
	}

	Result->Allocate(Num());

	FVoxelBufferIterator Iterator;
	Iterator.Initialize(Num(), 0);

	for (; Iterator; ++Iterator)
	{
		FVoxelUtilities::Memcpy(
			Result->GetByteRawView_NotConstant(Iterator),
			GetByteRawView_NotConstant(Iterator));
	}

	return Result;
}

int32 FVoxelBufferStorage::AddUninitialized(const int32 NumToAdd)
{
	VOXEL_FUNCTION_COUNTER_NUM(NumToAdd, 1024);
	check(bCanGrow);

	if (!ensure(NumToAdd > 0))
	{
		return -1;
	}

	const int32 OldNum = ArrayNum;
	const int32 NewNum = ArrayNum + NumToAdd;
	ArrayNum = NewNum;

	const int32 OldNumChunks = FMath::DivideAndRoundUp(OldNum, NumPerChunk);
	const int32 NewNumChunks = FMath::DivideAndRoundUp(NewNum, NumPerChunk);

	Chunks.Reserve(NewNumChunks + 1);
	if (Chunks.Num() > 0)
	{
		ensure(Chunks.Pop(false) == nullptr);
	}
	ensure(Chunks.Num() == OldNumChunks);

	for (int32 Index = OldNumChunks; Index < NewNumChunks; Index++)
	{
		Chunks.Add(AllocateChunk());
	}
	Chunks.Add(nullptr);
	ensure(Chunks.Num() == NewNumChunks + 1);

	AllocatedSizeTracker = GetAllocatedSize();

	return OldNum;
}

void FVoxelBufferStorage::AddZeroed(const int32 NumToAdd)
{
	VOXEL_FUNCTION_COUNTER_NUM(NumToAdd, 1024);
	check(bCanGrow);

	if (NumToAdd == 0)
	{
		return;
	}

	const int32 Index = AddUninitialized(NumToAdd);

	FVoxelBufferIterator Iterator;
	Iterator.Initialize(Num(), Index);
	for (; Iterator; ++Iterator)
	{
		FVoxelUtilities::Memzero(GetByteRawView_NotConstant(Iterator));
	}
}

void FVoxelBufferStorage::Append(const FVoxelBufferStorage& BufferStorage, const int32 BufferNum)
{
	VOXEL_FUNCTION_COUNTER_NUM(BufferStorage.Num(), 1024);
	check(bCanGrow);

	if (!ensure(BufferStorage.IsConstant() || BufferStorage.Num() == BufferNum))
	{
		return;
	}

	if (BufferNum == 0)
	{
		return;
	}

	const int32 Index = AddUninitialized(BufferNum);
	if (BufferStorage.IsConstant())
	{
		FVoxelBufferIterator Iterator;
		Iterator.Initialize(Num(), Index);

		for (; Iterator; ++Iterator)
		{
			VOXEL_SWITCH_TERMINAL_TYPE_SIZE(TypeSize)
			{
				using Type = VOXEL_GET_TYPE(TypeInstance);

				FVoxelUtilities::SetAll(
					this->As<Type>().GetRawView_NotConstant(Iterator),
					BufferStorage.As<Type>().GetConstant());
			};
		}

		return;
	}

	TVoxelBufferMultiIterator<2> Iterator(Num(), BufferStorage.Num());
	Iterator.SetIndex<0>(Index);
	Iterator.SetIndex<1>(0);

	for (; Iterator; ++Iterator)
	{
		FVoxelUtilities::Memcpy(
			GetByteRawView_NotConstant(Iterator.Get<0>()),
			BufferStorage.GetByteRawView_NotConstant(Iterator.Get<1>()));
	}
}

void FVoxelBufferStorage::CopyTo(const TVoxelArrayView<uint8> OtherData) const
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(OtherData.Num() % TypeSize == 0))
	{
		return;
	}

	if (IsConstant())
	{
		VOXEL_SWITCH_TERMINAL_TYPE_SIZE(TypeSize)
		{
			using Type = VOXEL_GET_TYPE(TypeInstance);

			FVoxelUtilities::SetAll(
				ReinterpretCastVoxelArrayView<Type>(OtherData),
				this->As<Type>().GetConstant());
		};
		return;
	}

	if (!ensure(OtherData.Num() / TypeSize == Num()))
	{
		return;
	}

	ForeachVoxelBufferChunk(Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		VOXEL_SWITCH_TERMINAL_TYPE_SIZE(TypeSize)
		{
			using Type = VOXEL_GET_TYPE(TypeInstance);

			FVoxelUtilities::Memcpy(
				ReinterpretCastVoxelArrayView<Type>(OtherData).Slice(Iterator.GetIndex(), Iterator.Num()),
				this->As<Type>().GetRawView_NotConstant(Iterator));
		};
	});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Iterator functions are not perf critical, no need to inline them

void FVoxelBufferStorage::CheckIterator(const FVoxelBufferIterator& Iterator) const
{
	check(Iterator.IsValid());
	check(0 <= Iterator.ChunkOffset && Iterator.ChunkOffset < NumPerChunk);
	check(Iterator.TotalNum <= Align(Num(), 8) || IsConstant());
}

uint8* FVoxelBufferStorage::GetByteData(const FVoxelBufferIterator& Iterator)
{
	CheckIterator(Iterator);

	if (IsConstant())
	{
		check(IsConstant());
		check(Chunks[0]);
		return static_cast<uint8*>(Chunks[0]);
	}

	uint8* Chunk = static_cast<uint8*>(Chunks[Iterator.ChunkIndex]);
	check(Chunk);
	return Chunk + Iterator.ChunkOffset * TypeSize;
}

const uint8* FVoxelBufferStorage::GetByteData(const FVoxelBufferIterator& Iterator) const
{
	return ConstCast(this)->GetByteData(Iterator);
}

TVoxelArrayView<uint8> FVoxelBufferStorage::GetByteRawView_NotConstant(const FVoxelBufferIterator& Iterator)
{
	check(Iterator.TotalNum <= Align(Num(), 8));
	return TVoxelArrayView<uint8>(GetByteData(Iterator), Iterator.Num() * TypeSize);
}

TConstVoxelArrayView<uint8> FVoxelBufferStorage::GetByteRawView_NotConstant(const FVoxelBufferIterator& Iterator) const
{
	return ConstCast(this)->GetByteRawView_NotConstant(Iterator);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelBufferStorage::AddUninitialized_Allocate()
{
	checkVoxelSlow(ArrayNum % NumPerChunk == 0);

	if (Chunks.Num() == 0)
	{
		checkVoxelSlow(ArrayNum == 0);
		Chunks.Reserve(2);
		Chunks.Add(nullptr);
	}

	checkVoxelSlow(!Chunks.Last());
	Chunks.Last() = AllocateChunk();
	Chunks.Add(nullptr);

	AllocatedSizeTracker = GetAllocatedSize();
}

void* FVoxelBufferStorage::AllocateChunk(const int32 Num) const
{
	check(Num <= NumPerChunk);
	check(Num == NumPerChunk || !bCanGrow);

	void* Chunk = FVoxelMemory::Malloc(Num * TypeSize, Alignment);

	if (GVoxelCheckNaNs)
	{
		VOXEL_SCOPE_COUNTER("Memzero");
		FMemory::Memzero(Chunk, Num * TypeSize);
	}

	return Chunk;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void TVoxelBufferStorage<float>::FixupSignBit()
{
	VOXEL_FUNCTION_COUNTER_NUM(Num(), 4096);

	ForeachVoxelBufferChunk(Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelBufferStorage_FixupSignBit(
			GetData(Iterator),
			Iterator.Num());
	});
}

FFloatInterval TVoxelBufferStorage<float>::GetMinMaxSafe() const
{
	VOXEL_FUNCTION_COUNTER_NUM(Num(), 4096);

	if (IsConstant())
	{
		return FFloatInterval(GetConstant(), GetConstant());
	}

	FFloatInterval MinMax{ MAX_flt, -MAX_flt };
	for (const FVoxelBufferIterator& Iterator : MakeVoxelBufferIterator(Num()))
	{
		const FFloatInterval IteratorMinMax = FVoxelUtilities::GetMinMaxSafe(GetRawView_NotConstant(Iterator));
		MinMax.Min = FMath::Min(MinMax.Min, IteratorMinMax.Min);
		MinMax.Max = FMath::Max(MinMax.Max, IteratorMinMax.Max);
	}
	return MinMax;
}