// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelMaterial.h"
#include "VoxelMaterialNodes.generated.h"

USTRUCT(Category = "Material")
struct VOXELGRAPHCORE_API FVoxelNode_MakeMaterial : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelMaterial, Material, nullptr);
	VOXEL_INPUT_PIN_ARRAY(FVoxelMaterialParameter, Parameter, nullptr, 1);
	VOXEL_OUTPUT_PIN(FVoxelMaterial, OutMaterial);
};