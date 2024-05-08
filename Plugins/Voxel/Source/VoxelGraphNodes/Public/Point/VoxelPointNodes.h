// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "Buffer/VoxelBaseBuffers.h"
#include "Point/VoxelChunkedPointSet.h"
#include "VoxelPointNodes.generated.h"

USTRUCT(Category = "Point", meta = (ShowInShortList))
struct VOXELGRAPHNODES_API FVoxelNode_GetPoints : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelChunkedPointSet, In, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelPointSet, Out);
};

USTRUCT(Category = "Point")
struct VOXELGRAPHNODES_API FVoxelNode_FilterPoints : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelPointSet, In, nullptr);
	VOXEL_INPUT_PIN(FVoxelBoolBuffer, KeepPoint, true);
	VOXEL_OUTPUT_PIN(FVoxelPointSet, Out);
};

USTRUCT(Category = "Point")
struct VOXELGRAPHNODES_API FVoxelNode_DensityFilter : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelPointSet, In, nullptr);
	VOXEL_INPUT_PIN(FVoxelFloatBuffer, Density, 1.f);
	VOXEL_INPUT_PIN(FVoxelSeed, Seed, nullptr, AdvancedDisplay);
	VOXEL_OUTPUT_PIN(FVoxelPointSet, Out);
};