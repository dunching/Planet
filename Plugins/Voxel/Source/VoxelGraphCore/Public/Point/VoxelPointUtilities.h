// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "Buffer/VoxelStaticMeshBuffer.h"

struct VOXELGRAPHCORE_API FVoxelPointUtilities
{
	static void GetMeshBounds(
		const FVoxelStaticMeshBuffer& MeshBuffer,
		const FVoxelVectorBuffer& PositionBuffer,
		const FVoxelQuaternionBuffer& RotationBuffer,
		const FVoxelVectorBuffer& ScaleBuffer,
		FVoxelVectorBuffer& OutBoundsMin,
		FVoxelVectorBuffer& OutBoundsMax);
};