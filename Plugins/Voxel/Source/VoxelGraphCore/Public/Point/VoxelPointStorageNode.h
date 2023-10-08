// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "Point/VoxelPointSet.h"
#include "VoxelPointStorageNode.generated.h"

USTRUCT(Category = "Point")
struct VOXELGRAPHCORE_API FVoxelNode_ApplyPointStorage : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelPointSet, In, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelPointSet, Out);
};