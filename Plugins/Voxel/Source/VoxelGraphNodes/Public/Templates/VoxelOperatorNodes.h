// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelISPCNode.h"
#include "VoxelTemplateNode.h"
#include "VoxelOperatorNodes.generated.h"

USTRUCT(meta = (Abstract))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_OperatorBase : public FVoxelTemplateNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

public:
	virtual FPin* ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const override;

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif

	virtual UScriptStruct* GetFloatInnerNode() const VOXEL_PURE_VIRTUAL({});
	virtual UScriptStruct* GetDoubleInnerNode() const VOXEL_PURE_VIRTUAL({});
	virtual UScriptStruct* GetInt32InnerNode() const VOXEL_PURE_VIRTUAL({});
};

USTRUCT(meta = (Abstract))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_UnaryOperator : public FVoxelTemplateNode_OperatorBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVoxelWildcard, Result);
};

USTRUCT(meta = (Abstract))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_BinaryOperator : public FVoxelTemplateNode_OperatorBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVoxelWildcard, Result);
};

USTRUCT(meta = (Abstract))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_CommutativeAssociativeOperator : public FVoxelTemplateNode_OperatorBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN_ARRAY(FVoxelWildcard, Input, nullptr, 2);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVoxelWildcard, Result);

public:
#if WITH_EDITOR
	class FDefinition : public Super::FDefinition
	{
	public:
		GENERATED_VOXEL_NODE_DEFINITION_BODY(FVoxelTemplateNode_CommutativeAssociativeOperator);

		virtual bool CanAddInputPin() const override
		{
			return CanAddToCategory(Node.InputPins);
		}
		virtual void AddInputPin() override;

		virtual bool CanRemoveInputPin() const override
		{
			return CanRemoveFromCategory(Node.InputPins);
		}
		virtual void RemoveInputPin() override
		{
			RemoveFromCategory(Node.InputPins);
		}
	};
#endif
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Add_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} + {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Add_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} + {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Add_Int : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} + {B}";
	}
};

// Addition (A + B)
USTRUCT(Category = "Math|Operators", meta = (CompactNodeTitle = "+", Keywords = "+ add plus", Operator = "+"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Add : public FVoxelTemplateNode_CommutativeAssociativeOperator
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetFloatInnerNode() const override
	{
		return FVoxelNode_Add_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleInnerNode() const override
	{
		return FVoxelNode_Add_Double::StaticStruct();
	}
	virtual UScriptStruct* GetInt32InnerNode() const override
	{
		return FVoxelNode_Add_Int::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Subtract_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} - {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Subtract_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} - {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Subtract_Int : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} - {B}";
	}
};

// Subtraction (A - B)
USTRUCT(Category = "Math|Operators", meta = (CompactNodeTitle = "-", Keywords = "- subtract minus", Operator = "-"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Subtract : public FVoxelTemplateNode_BinaryOperator
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetFloatInnerNode() const override
	{
		return FVoxelNode_Subtract_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleInnerNode() const override
	{
		return FVoxelNode_Subtract_Double::StaticStruct();
	}
	virtual UScriptStruct* GetInt32InnerNode() const override
	{
		return FVoxelNode_Subtract_Int::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Multiply_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} * {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Multiply_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} * {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Multiply_Int : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} * {B}";
	}
};

// Multiplication (A * B)
USTRUCT(Category = "Math|Operators", meta = (CompactNodeTitle = "*", Keywords = "* multiply", Operator = "*"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Multiply : public FVoxelTemplateNode_CommutativeAssociativeOperator
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetFloatInnerNode() const override
	{
		return FVoxelNode_Multiply_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleInnerNode() const override
	{
		return FVoxelNode_Multiply_Double::StaticStruct();
	}
	virtual UScriptStruct* GetInt32InnerNode() const override
	{
		return FVoxelNode_Multiply_Int::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Divide_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		// Make sure to only generate finite numbers
		// This is mainly to not trigger nan checks, but also
		// because nan behavior might differ based on hardware
		// (typically ARM vs x86)
		return
			"{ReturnValue} = {B} != 0.f "
			"? {A} / {B} "
			// + / 0 = inf
			": {A} > 0.f ? BIG_NUMBER "
			// - / 0 = -inf
			": {A} < 0.f ? -BIG_NUMBER "
			// 0 / 0 = 1
			": 1.f";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Divide_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} / {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Divide_Int : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "IGNORE_PERF_WARNING\n{ReturnValue} = {B} != 0 ? {A} / {B} : 0";
	}
};

// Division (A / B)
USTRUCT(Category = "Math|Operators", meta = (CompactNodeTitle = "/", Keywords = "/ divide division", Operator = "/"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Divide : public FVoxelTemplateNode_BinaryOperator
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetFloatInnerNode() const override
	{
		return FVoxelNode_Divide_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleInnerNode() const override
	{
		return FVoxelNode_Divide_Double::StaticStruct();
	}
	virtual UScriptStruct* GetInt32InnerNode() const override
	{
		return FVoxelNode_Divide_Int::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Modulus : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "IGNORE_PERF_WARNING\n{ReturnValue} = {B} != 0 ? {A} % {B} : 0";
	}
};

USTRUCT(Category = "Math|Operators", meta = (DisplayName = "% (Integer)", CompactNodeTitle = "%", Keywords = "% modulus", Operator = "%"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Modulus : public FVoxelTemplateNode_BinaryOperator
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetFloatInnerNode() const override
	{
		return nullptr;
	}
	virtual UScriptStruct* GetDoubleInnerNode() const override
	{
		return nullptr;
	}
	virtual UScriptStruct* GetInt32InnerNode() const override
	{
		return FVoxelNode_Modulus::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Min_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = min({A}, {B})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Min_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = min({A}, {B})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Min_Int : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = min({A}, {B})";
	}
};

USTRUCT(Category = "Math|Operators", meta = (CompactNodeTitle = "MIN"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Min : public FVoxelTemplateNode_CommutativeAssociativeOperator
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetFloatInnerNode() const override
	{
		return FVoxelNode_Min_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleInnerNode() const override
	{
		return FVoxelNode_Min_Double::StaticStruct();
	}
	virtual UScriptStruct* GetInt32InnerNode() const override
	{
		return FVoxelNode_Min_Int::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Max_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = max({A}, {B})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Max_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = max({A}, {B})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Max_Int : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = max({A}, {B})";
	}
};

USTRUCT(Category = "Math|Operators", meta = (CompactNodeTitle = "MAX"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Max : public FVoxelTemplateNode_CommutativeAssociativeOperator
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetFloatInnerNode() const override
	{
		return FVoxelNode_Max_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleInnerNode() const override
	{
		return FVoxelNode_Max_Double::StaticStruct();
	}
	virtual UScriptStruct* GetInt32InnerNode() const override
	{
		return FVoxelNode_Max_Int::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Abs_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = abs({Value})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Abs_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = abs({Value})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Abs_Int : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = abs({Value})";
	}
};

// Returns the absolute (positive) value
USTRUCT(Category = "Math|Operators", meta = (CompactNodeTitle = "ABS"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Abs : public FVoxelTemplateNode_UnaryOperator
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetFloatInnerNode() const override
	{
		return FVoxelNode_Abs_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleInnerNode() const override
	{
		return FVoxelNode_Abs_Double::StaticStruct();
	}
	virtual UScriptStruct* GetInt32InnerNode() const override
	{
		return FVoxelNode_Abs_Int::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_OneMinus_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = 1 - {Value}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_OneMinus_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = 1 - {Value}";
	}
};

USTRUCT(Category = "Math|Operators", meta = (CompactNodeTitle = "1-X"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_OneMinus : public FVoxelTemplateNode_UnaryOperator
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetFloatInnerNode() const override
	{
		return FVoxelNode_OneMinus_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleInnerNode() const override
	{
		return FVoxelNode_OneMinus_Double::StaticStruct();
	}
	virtual UScriptStruct* GetInt32InnerNode() const override
	{
		return nullptr;
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Sign_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {Value} < 0.f ? -1.f : {Value} > 0.f ? 1.f : 0.f";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Sign_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {Value} < 0.d ? -1.d : {Value} > 0.f ? 1.d : 0.d";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Sign_Int : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, Value, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {Value} < 0 ? -1 : {Value} > 0 ? 1 : 0";
	}
};

// Sign (return -1 if A < 0, 0 if A is zero, and +1 if A > 0)
USTRUCT(Category = "Math|Operators", meta = (CompactNodeTitle = "Sign"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Sign : public FVoxelTemplateNode_UnaryOperator
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetFloatInnerNode() const override
	{
		return FVoxelNode_Sign_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleInnerNode() const override
	{
		return FVoxelNode_Sign_Double::StaticStruct();
	}
	virtual UScriptStruct* GetInt32InnerNode() const override
	{
		return FVoxelNode_Sign_Int::StaticStruct();
	}
};