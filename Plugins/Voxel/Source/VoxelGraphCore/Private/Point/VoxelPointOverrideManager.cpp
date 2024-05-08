// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Point/VoxelPointOverrideManager.h"

TSharedRef<FVoxelPointOverrideChunk> FVoxelPointOverrideManager::FindOrAddChunk(const FVoxelPointChunkRef& ChunkRef)
{
	VOXEL_SCOPE_LOCK(CriticalSection);

	TSharedPtr<FVoxelPointOverrideChunk>& ChunkData = ChunkRefToChunk_RequiresLock.FindOrAdd(ChunkRef);
	if (!ChunkData)
	{
		ChunkData = MakeVoxelShared<FVoxelPointOverrideChunk>();
	}
	return ChunkData.ToSharedRef();
}

void FVoxelPointOverrideManager::AddReferencedObjects(FReferenceCollector& Collector)
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);

	for (auto It = ChunkRefToChunk_RequiresLock.CreateIterator(); It; ++It)
	{
		if (!It.Value().IsUnique())
		{
			continue;
		}

		{
			VOXEL_SCOPE_LOCK(It.Value()->CriticalSection);

			if (It.Value()->PointIdsToHide_RequiresLock.Num() > 0)
			{
				continue;
			}
		}

		// Unique and nothing stored in it: safe to remove
		It.RemoveCurrent();
	}
}