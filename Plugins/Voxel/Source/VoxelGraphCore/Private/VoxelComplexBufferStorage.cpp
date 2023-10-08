// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelComplexBufferStorage.h"

DEFINE_VOXEL_MEMORY_STAT(STAT_VoxelComplexBufferStorage);

void FVoxelComplexBufferStorage::Allocate(const int32 Num)
{
	VOXEL_FUNCTION_COUNTER_NUM(Num, 128);
	check(Num >= 0);
	check(InnerStruct);
	check(TypeSize > 0);
	checkf(ArrayNum == 0, TEXT("Buffer already allocated"));
	check(Chunks.Num() == 0);

	AddZeroed(Num);
}

void FVoxelComplexBufferStorage::Empty()
{
	VOXEL_FUNCTION_COUNTER();

	if (InnerStruct->GetCppStructOps()->HasDestructor())
	{
		VOXEL_SCOPE_COUNTER_FORMAT_COND(Num() > 128, "Destruct %s Num=%d", *InnerStruct->GetName(), Num());
		for (int32 Index = 0; Index < Num(); Index++)
		{
			InnerStruct->GetCppStructOps()->Destruct((*this)[Index].GetMemory());
		}
	}

	ArrayNum = 0;

	for (void* Chunk : Chunks)
	{
		check(Chunk);
		FVoxelMemory::Free(Chunk);
	}
	Chunks.Empty();

	AllocatedSizeTracker = GetAllocatedSize();
}

int64 FVoxelComplexBufferStorage::GetAllocatedSize() const
{
	return
		Chunks.GetAllocatedSize() +
		Chunks.Num() * NumPerChunk * TypeSize;
}

void FVoxelComplexBufferStorage::SetConstant(const FConstVoxelStructView Struct)
{
	Allocate(1);
	Struct.CopyTo((*this)[0]);
}

int32 FVoxelComplexBufferStorage::AddZeroed(const int32 NumToAdd)
{
	VOXEL_FUNCTION_COUNTER_NUM(NumToAdd, 128);
	check(NumToAdd >= 0);

	if (NumToAdd == 0)
	{
		return Num();
	}

	const int32 OldNum = ArrayNum;
	const int32 NewNum = ArrayNum + NumToAdd;
	ArrayNum = NewNum;

	const int32 OldNumChunks = FMath::DivideAndRoundUp(OldNum, NumPerChunk);
	const int32 NewNumChunks = FMath::DivideAndRoundUp(NewNum, NumPerChunk);

	Chunks.Reserve(NewNumChunks);
	ensure(Chunks.Num() == OldNumChunks);

	for (int32 Index = OldNumChunks; Index < NewNumChunks; Index++)
	{
		Chunks.Add(AllocateChunk());
	}
	ensure(Chunks.Num() == NewNumChunks);

	if (!InnerStruct->GetCppStructOps()->HasZeroConstructor())
	{
		VOXEL_SCOPE_COUNTER_FORMAT_COND(NumToAdd > 128, "Construct %s Num=%d", *InnerStruct->GetName(), NumToAdd);
		for (int32 Index = OldNum; Index < NewNum; Index++)
		{
			InnerStruct->GetCppStructOps()->Construct((*this)[Index].GetMemory());
		}
	}

	AllocatedSizeTracker = GetAllocatedSize();

	return OldNum;
}

void FVoxelComplexBufferStorage::BulkAdd(const FConstVoxelStructView Value, const int32 NumToAdd)
{
	VOXEL_FUNCTION_COUNTER_NUM(NumToAdd, 128);

	const int32 OldNum = AddZeroed(NumToAdd);

	VOXEL_SCOPE_COUNTER_FORMAT_COND(NumToAdd > 128, "Copy %s Num=%d", *InnerStruct->GetName(), NumToAdd);
	for (int32 Index = OldNum; Index < Num(); Index++)
	{
		Value.CopyTo((*this)[Index]);
	}
}

void FVoxelComplexBufferStorage::Append(const FVoxelComplexBufferStorage& Other, const int32 Num)
{
	VOXEL_FUNCTION_COUNTER_NUM(Num, 128);

	const int32 OldNum = AddZeroed(Num);

	VOXEL_SCOPE_COUNTER_FORMAT_COND(Num > 128, "Copy %s Num=%d", *InnerStruct->GetName(), Num);
	for (int32 Index = 0; Index < Num; Index++)
	{
		Other[Index].CopyTo((*this)[OldNum + Index]);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void* FVoxelComplexBufferStorage::AllocateChunk() const
{
	check(TypeSize > 0);

	void* Chunk = FVoxelMemory::Malloc(NumPerChunk * TypeSize);
	FMemory::Memzero(Chunk, NumPerChunk * TypeSize);
	return Chunk;
}