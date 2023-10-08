// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"

DECLARE_VOXEL_MEMORY_STAT(VOXELGRAPHCORE_API, STAT_VoxelComplexBufferStorage, "Complex Buffer Storage");

class VOXELGRAPHCORE_API FVoxelComplexBufferStorage
{
public:
	static constexpr int32 NumPerChunk = 128;

	explicit FVoxelComplexBufferStorage(UScriptStruct* InnerStruct)
		: InnerStruct(InnerStruct)
	{
		checkVoxelSlow(InnerStruct);
		checkVoxelSlow(InnerStruct->GetStructureSize() > 0);
		checkVoxelSlow(InnerStruct->GetMinAlignment() <= 16);
		TypeSize = InnerStruct->GetStructureSize();
	}
	FVoxelComplexBufferStorage(const FVoxelComplexBufferStorage&) = delete;

	FORCEINLINE ~FVoxelComplexBufferStorage()
	{
		Empty();
	}

	FORCEINLINE int32 Num() const
	{
		return ArrayNum;
	}
	FORCEINLINE UScriptStruct* GetInnerStruct() const
	{
		return InnerStruct;
	}
	FORCEINLINE bool IsConstant() const
	{
		return Num() == 1;
	}
	FORCEINLINE bool IsValidIndex(const int32 Index) const
	{
		return 0 <= Index && (IsConstant() || Index < ArrayNum);
	}

	void Allocate(int32 Num);
	void Empty();
	int64 GetAllocatedSize() const;
	void SetConstant(FConstVoxelStructView Struct);
	int32 AddZeroed(int32 NumToAdd);
	void BulkAdd(FConstVoxelStructView Value, int32 NumToAdd);
	void Append(const FVoxelComplexBufferStorage& Other, int32 Num);

	FORCEINLINE FVoxelStructView operator[](int32 Index)
	{
		checkVoxelSlow(TypeSize > 0);
		checkVoxelSlow(IsValidIndex(Index));

		const int32 Mask = ArrayNum != 1;
		Index *= Mask;

		void* RESTRICT Chunk = Chunks[FVoxelUtilities::GetChunkIndex<NumPerChunk>(Index)];
		checkVoxelSlow(Chunk);

		void* Memory = static_cast<uint8*>(Chunk) + TypeSize * FVoxelUtilities::GetChunkOffset<NumPerChunk>(Index);
		return FVoxelStructView(InnerStruct, Memory);
	}
	FORCEINLINE FConstVoxelStructView operator[](const int32 Index) const
	{
		return ConstCast(this)->operator[](Index);
	}

	FORCEINLINE FConstVoxelStructView GetConstant() const
	{
		checkVoxelSlow(IsConstant());
		return (*this)[0];
	}

private:
	UScriptStruct* InnerStruct = nullptr;
	int32 TypeSize = 0;
	int32 ArrayNum = 0;
	TVoxelArray<void*, TVoxelInlineAllocator<2>> Chunks;

	void* AllocateChunk() const;

	VOXEL_ALLOCATED_SIZE_TRACKER_CUSTOM(STAT_VoxelComplexBufferStorage, AllocatedSizeTracker);
};