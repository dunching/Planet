// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "Point/VoxelPointSet.h"
#include "VoxelPruneByBoundsNode.generated.h"

USTRUCT(Category = "Point")
struct VOXELGRAPHNODES_API FVoxelNode_PruneByBounds : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelPointSet, PointsToPrune, nullptr);
	VOXEL_INPUT_PIN(FVoxelPointSet, PointsToCheck, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelPointSet, Out);
};