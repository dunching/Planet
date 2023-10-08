// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelISPCNode.h"
#include "VoxelTemplateNode.h"
#include "VoxelTemplatedMathNodes.generated.h"

USTRUCT(meta = (Abstract))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_FloatMathNode : public FVoxelTemplateNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_OUTPUT_PIN(FVoxelWildcard, ReturnValue);

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
struct VOXELGRAPHNODES_API FVoxelNode_Frac : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {Value} - floor({Value})";
	}
};

// Returns the fractional part of a float.
USTRUCT(Category = "Math|Float")
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Frac : public FVoxelTemplateNode_FloatMathNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, A, nullptr);

	virtual UScriptStruct* GetInnerNode() const override
	{
		return FVoxelNode_Frac::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct VOXELGRAPHNODES_API FVoxelNode_Power : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	// If negative, abs(Base) will be used instead
	VOXEL_TEMPLATE_INPUT_PIN(float, Base, nullptr);
	// Exponent
	VOXEL_TEMPLATE_INPUT_PIN(float, Exp, nullptr);
	// Result
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		// Base cannot be negative
		return "{ReturnValue} = pow(abs({Base}), {Exp})";
	}
};

// Power (Base to the Exp-th power)
USTRUCT(Category = "Math|Float")
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Power : public FVoxelTemplateNode_FloatMathNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, Base, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, Exp, nullptr);

	virtual UScriptStruct* GetInnerNode() const override
	{
		return FVoxelNode_Power::StaticStruct();
	}
};