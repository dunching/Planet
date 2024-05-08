// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "Point/VoxelPointSet.h"
#include "VoxelMergePointsNode.generated.h"

USTRUCT(Category = "Point")
struct VOXELGRAPHCORE_API FVoxelNode_MergePoints : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN_ARRAY(FVoxelPointSet, Input, nullptr, 2);
	VOXEL_OUTPUT_PIN(FVoxelPointSet, Out);
};