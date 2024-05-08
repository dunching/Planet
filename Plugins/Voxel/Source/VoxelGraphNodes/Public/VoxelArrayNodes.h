// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelArrayNodes.generated.h"

// Get the number of items in an array
USTRUCT(Category = "Array", meta = (DisplayName = "Length", CompactNodeTitle = "LENGTH", Keywords = "num size count"))
struct VOXELGRAPHNODES_API FVoxelNode_ArrayLength : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

public:
	VOXEL_INPUT_PIN(FVoxelWildcardBuffer, Values, nullptr, ArrayPin);
	VOXEL_OUTPUT_PIN(int32, Result);

	//~ Begin FVoxelNode Interface
#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif
	//~ End FVoxelNode Interface
};

// Given an array and an index, returns the item in the array at that index
USTRUCT(Category = "Array", meta = (DisplayName = "Get", CompactNodeTitle = "GET", Keywords = "item array"))
struct VOXELGRAPHNODES_API FVoxelNode_ArrayGetItem : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

public:
	VOXEL_INPUT_PIN(FVoxelWildcardBuffer, Values, nullptr, ArrayPin);
	VOXEL_INPUT_PIN(int32, Index, 0);
	VOXEL_OUTPUT_PIN(FVoxelWildcard, Result);

	//~ Begin FVoxelNode Interface
#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif
	//~ End FVoxelNode Interface
};