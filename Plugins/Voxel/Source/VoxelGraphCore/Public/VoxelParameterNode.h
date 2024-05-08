// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelParameterNode.generated.h"

USTRUCT(meta = (Internal))
struct FVoxelNode_Parameter : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	UPROPERTY()
	FGuid ParameterGuid;

	UPROPERTY()
	FName ParameterName;

	VOXEL_OUTPUT_PIN(FVoxelWildcard, Value);
};