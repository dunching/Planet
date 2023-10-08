// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelGetPreviousOutputNode.generated.h"

// Get a previous output if this is used in a recursive macro
USTRUCT(Category = "Misc")
struct VOXELGRAPHCORE_API FVoxelNode_GetPreviousOutput : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FName, Name, "MyOutput");
	VOXEL_INPUT_PIN(FVoxelWildcardBuffer, Default, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelWildcardBuffer, Value);

#if WITH_EDITOR
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif
};