// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelSubsystem.h"
#include "Point/VoxelPointChunkRef.h"
#include "Point/VoxelChunkedPointSet.h"
#include "VoxelPointSubsystem.generated.h"

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelPointSubsystem : public FVoxelSubsystem
{
	GENERATED_BODY()
	GENERATED_VOXEL_SUBSYSTEM_BODY(FVoxelPointSubsystem)

	FVoxelFastCriticalSection CriticalSection;
	TVoxelMap<FVoxelPointChunkProviderRef, TSharedPtr<const FVoxelChunkedPointSet>> ChunkProviderToChunkedPointSet_RequiresLock;
};