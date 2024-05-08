// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "VoxelGradientNodes.generated.h"

USTRUCT(Category = "Gradient")
struct VOXELGRAPHNODES_API FVoxelNode_GetGradient : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelFloatBuffer, Value, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelVectorBuffer, Gradient);
};