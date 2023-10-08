// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNavmesh.generated.h"

DECLARE_VOXEL_MEMORY_STAT(VOXELCORE_API, STAT_VoxelNavigationMeshMemory, "Voxel Navigation Mesh Memory");

USTRUCT()
struct VOXELCORE_API FVoxelNavmesh
{
	GENERATED_BODY()

	FVector Offset = FVector::ZeroVector;
	FVoxelBox LocalBounds;
	TVoxelArray<int32> Indices;
	TVoxelArray<FVector3f> Vertices;

	VOXEL_ALLOCATED_SIZE_TRACKER(STAT_VoxelNavigationMeshMemory);

	int64 GetAllocatedSize() const
	{
		return Indices.GetAllocatedSize() + Vertices.GetAllocatedSize();
	}
};