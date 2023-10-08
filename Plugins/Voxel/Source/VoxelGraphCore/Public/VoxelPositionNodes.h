// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "VoxelPositionNodes.generated.h"

USTRUCT(Category = "Misc")
struct VOXELGRAPHCORE_API FVoxelNode_QueryWithGradientStep : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, Data, nullptr);
	VOXEL_INPUT_PIN(float, Step, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVoxelWildcard, OutData);

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif
};

USTRUCT(Category = "Misc")
struct VOXELGRAPHCORE_API FVoxelNode_QueryWithPosition : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, Data, nullptr);
	VOXEL_INPUT_PIN(FVoxelVectorBuffer, Position, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVoxelWildcard, OutData);

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif
};