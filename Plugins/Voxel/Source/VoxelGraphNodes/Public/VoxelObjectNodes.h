// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "Buffer/VoxelBaseBuffers.h"
#include "VoxelObjectNodes.generated.h"

USTRUCT(Category = "Misc", meta = (DisplayName = "Is Valid"))
struct VOXELGRAPHNODES_API FVoxelNode_IsValidObject : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, Object, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, Result);

	//~ Begin FVoxelNode Interface
	virtual bool IsPureNode() const override { return true; }

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
#endif
	//~ End FVoxelNode Interface
};

USTRUCT(meta = (Abstract))
struct VOXELGRAPHNODES_API FVoxelNode_EqualBase_Object : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, Result);

	//~ Begin FVoxelNode Interface
	virtual bool IsPureNode() const override { return true; }

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif
	//~ End FVoxelNode Interface

	virtual bool GetDifferentTypesValues() const VOXEL_PURE_VIRTUAL(false);
	virtual bool EqualityCheck(const TWeakObjectPtr<UObject>& A, const TWeakObjectPtr<UObject>& B) const VOXEL_PURE_VIRTUAL(false);
};

USTRUCT(meta = (Internal))
struct VOXELGRAPHNODES_API FVoxelNode_Equal_Object : public FVoxelNode_EqualBase_Object
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	//~ Begin FVoxelNode Interface
	virtual bool GetDifferentTypesValues() const override { return false; }
	virtual bool EqualityCheck(const TWeakObjectPtr<UObject>& A, const TWeakObjectPtr<UObject>& B) const override;
	//~ End FVoxelNode Interface
};

USTRUCT(meta = (Internal))
struct VOXELGRAPHNODES_API FVoxelNode_NotEqual_Object : public FVoxelNode_EqualBase_Object
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	//~ Begin FVoxelNode Interface
	virtual bool GetDifferentTypesValues() const override { return true; }
	virtual bool EqualityCheck(const TWeakObjectPtr<UObject>& A, const TWeakObjectPtr<UObject>& B) const override;
	//~ End FVoxelNode Interface
};