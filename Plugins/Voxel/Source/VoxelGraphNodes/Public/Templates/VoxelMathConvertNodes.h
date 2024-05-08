// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelISPCNode.h"
#include "VoxelTemplateNode.h"
#include "VoxelMathConvertNodes.generated.h"

USTRUCT(meta = (Abstract))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_AbstractMathConvert : public FVoxelTemplateNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVoxelWildcard, Result);

public:
	virtual FPin* ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const override;

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif

	virtual UScriptStruct* GetFloatToFloatInnerNode() const VOXEL_PURE_VIRTUAL({});
	virtual UScriptStruct* GetDoubleToDoubleInnerNode() const VOXEL_PURE_VIRTUAL({});
	virtual UScriptStruct* GetFloatToInt32InnerNode() const VOXEL_PURE_VIRTUAL({});
	virtual UScriptStruct* GetDoubleToInt32InnerNode() const VOXEL_PURE_VIRTUAL({});
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Ceil_FloatToFloat : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = ceil({Value})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Ceil_DoubleToDouble : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = ceil({Value})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Ceil_FloatToInt : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = (int)ceil({Value})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Ceil_DoubleToInt : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = (int)ceil({Value})";
	}
};

// Rounds decimal values upwards
USTRUCT(Category = "Math|Operators")
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Ceil : public FVoxelTemplateNode_AbstractMathConvert
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetFloatToFloatInnerNode() const override
	{
		return FVoxelNode_Ceil_FloatToFloat::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleToDoubleInnerNode() const override
	{
		return FVoxelNode_Ceil_DoubleToDouble::StaticStruct();
	}
	virtual UScriptStruct* GetFloatToInt32InnerNode() const override
	{
		return FVoxelNode_Ceil_FloatToInt::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleToInt32InnerNode() const override
	{
		return FVoxelNode_Ceil_DoubleToInt::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Round_FloatToFloat : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = round({Value})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Round_DoubleToDouble : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = round({Value})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Round_FloatToInt : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = (int)round({Value})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Round_DoubleToInt : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = (int)round({Value})";
	}
};

USTRUCT(Category = "Math|Operators")
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Round : public FVoxelTemplateNode_AbstractMathConvert
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetFloatToFloatInnerNode() const override
	{
		return FVoxelNode_Round_FloatToFloat::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleToDoubleInnerNode() const override
	{
		return FVoxelNode_Round_DoubleToDouble::StaticStruct();
	}
	virtual UScriptStruct* GetFloatToInt32InnerNode() const override
	{
		return FVoxelNode_Round_FloatToInt::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleToInt32InnerNode() const override
	{
		return FVoxelNode_Round_DoubleToInt::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Floor_FloatToFloat : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = floor({Value})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Floor_DoubleToDouble : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = floor({Value})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Floor_FloatToInt : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = (int)floor({Value})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Floor_DoubleToInt : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = (int)floor({Value})";
	}
};

USTRUCT(Category = "Math|Operators")
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Floor : public FVoxelTemplateNode_AbstractMathConvert
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetFloatToFloatInnerNode() const override
	{
		return FVoxelNode_Floor_FloatToFloat::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleToDoubleInnerNode() const override
	{
		return FVoxelNode_Floor_DoubleToDouble::StaticStruct();
	}
	virtual UScriptStruct* GetFloatToInt32InnerNode() const override
	{
		return FVoxelNode_Floor_FloatToInt::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleToInt32InnerNode() const override
	{
		return FVoxelNode_Floor_DoubleToInt::StaticStruct();
	}
};