// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelISPCNode.h"
#include "VoxelTemplateNode.h"
#include "VoxelLerpNodes.generated.h"

USTRUCT(meta = (Abstract))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_LerpBase : public FVoxelTemplateNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, B, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, Alpha, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVoxelWildcard, Result);

public:
	virtual FPin* ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const override;

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif

	virtual UScriptStruct* GetInnerNode() const VOXEL_PURE_VIRTUAL({});
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Lerp : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, B, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, Alpha, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = lerp({A}, {B}, {Alpha})";
	}
};

// Linearly interpolates between A and B based on Alpha (100% of A when Alpha=0, 100% of B when Alpha=1, 200% of B when Alpha=2)
USTRUCT(Category = "Math|Misc")
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Lerp : public FVoxelTemplateNode_LerpBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetInnerNode() const override
	{
		return FVoxelNode_Lerp::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_SafeLerp : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, B, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, Alpha, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = lerp({A}, {B}, clamp({Alpha}, 0.f, 1.f))";
	}
};

// Linearly interpolates between A and B based on clamped Alpha (100% of A when Alpha=0, 100% of B when Alpha=1, 100% of B when Alpha=2
USTRUCT(Category = "Math|Misc")
struct VOXELGRAPHNODES_API FVoxelTemplateNode_SafeLerp : public FVoxelTemplateNode_LerpBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetInnerNode() const override
	{
		return FVoxelNode_SafeLerp::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct VOXELGRAPHNODES_API FVoxelNode_SmoothStep : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, A, 0.f);
	VOXEL_TEMPLATE_INPUT_PIN(float, B, 1.f);
	VOXEL_TEMPLATE_INPUT_PIN(float, Alpha, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = SmoothStep({A}, {B}, {Alpha})";
	}
};

// Returns a smooth Hermite interpolation between 0 and 1 for the value X (where X ranges between A and B) Clamped to 0 for X <= A and 1 for X >= B
USTRUCT(Category = "Math|Misc")
struct VOXELGRAPHNODES_API FVoxelTemplateNode_SmoothStep : public FVoxelTemplateNode_LerpBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetInnerNode() const override
	{
		return FVoxelNode_SmoothStep::StaticStruct();
	}
};