// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "Point/VoxelPointHandle.h"

class VOXELGRAPHCORE_API FVoxelPointOverrideChunk
{
public:
	FVoxelFastCriticalSection CriticalSection;
	TMulticastDelegate<void(TConstVoxelArrayView<FVoxelPointId> PointIds)> OnChanged_RequiresLock;
	TVoxelSet<FVoxelPointId> PointIdsToHide_RequiresLock;
};

class VOXELGRAPHCORE_API FVoxelPointOverrideManager : public IVoxelWorldSubsystem
{
public:
	GENERATED_VOXEL_WORLD_SUBSYSTEM_BODY(FVoxelPointOverrideManager);

	TSharedRef<FVoxelPointOverrideChunk> FindOrAddChunk(const FVoxelPointChunkRef& ChunkRef);

	//~ Begin IVoxelWorldSubsystem Interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	//~ End IVoxelWorldSubsystem Interface

private:
	mutable FVoxelFastCriticalSection CriticalSection;
	TVoxelMap<FVoxelPointChunkRef, TSharedPtr<FVoxelPointOverrideChunk>> ChunkRefToChunk_RequiresLock;
};