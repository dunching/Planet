// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "Point/VoxelPointSpawner.h"
#include "VoxelScatterPointsNode.generated.h"

// Scatter points around parent points
// Output is only child points, not parents
USTRUCT(Category = "Point")
struct VOXELGRAPHNODES_API FVoxelNode_ScatterPoints : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelPointSet, In, nullptr);
	VOXEL_INPUT_PIN(FVoxelPointSpawner, PointSpawner, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelPointSet, Out);
};