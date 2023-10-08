// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelLocalVariableNodes.generated.h"

USTRUCT(meta = (Internal))
struct VOXELGRAPHCORE_API FVoxelLocalVariableDeclaration : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	UPROPERTY()
	FGuid Guid;

	VOXEL_INPUT_PIN(FVoxelWildcard, InputPin, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelWildcard, OutputPin);
};

USTRUCT(meta = (Internal))
struct VOXELGRAPHCORE_API FVoxelLocalVariableUsage : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	UPROPERTY()
	FGuid Guid;

	VOXEL_OUTPUT_PIN(FVoxelWildcard, OutputPin);
};