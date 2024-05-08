// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelISPCNode.h"
#include "VoxelObjectNodes.h"
#include "VoxelTemplateNode.h"
#include "Buffer/VoxelBaseBuffers.h"
#include "VoxelBoolNodes.generated.h"

USTRUCT(meta = (Abstract))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_EqualityBase : public FVoxelTemplateNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, Result);

public:
	virtual FPin* ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const override;

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif

	virtual UScriptStruct* GetBoolInnerNode() const VOXEL_PURE_VIRTUAL({});
	virtual UScriptStruct* GetByteInnerNode() const VOXEL_PURE_VIRTUAL({});
	virtual UScriptStruct* GetFloatInnerNode() const VOXEL_PURE_VIRTUAL({});
	virtual UScriptStruct* GetDoubleInnerNode() const VOXEL_PURE_VIRTUAL({});
	virtual UScriptStruct* GetInt32InnerNode() const VOXEL_PURE_VIRTUAL({});
	virtual UScriptStruct* GetObjectInnerNode() const VOXEL_PURE_VIRTUAL({});
	virtual UScriptStruct* GetConnectionInnerNode() const VOXEL_PURE_VIRTUAL({});
};

USTRUCT(meta = (Abstract))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_MultiInputBooleanNode : public FVoxelTemplateNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN_ARRAY(bool, Input, false, 2);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, Result);

public:
	virtual bool ShowPromotablePinsAsWildcards() const override
	{
		return false;
	}

	virtual FPin* ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const override;

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif

	virtual UScriptStruct* GetBooleanNode() const VOXEL_PURE_VIRTUAL({});

public:
#if WITH_EDITOR
	struct FDefinition : public Super::FDefinition
	{
		GENERATED_VOXEL_NODE_DEFINITION_BODY(FVoxelTemplateNode_MultiInputBooleanNode);

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

USTRUCT(meta = (Abstract))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_EqualitySingleDimension : public FVoxelTemplateNode_EqualityBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
#endif
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_BooleanAND : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(bool, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(bool, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} && {B}";
	}
};

// Returns the logical AND of two values (A AND B)
USTRUCT(Category = "Math|Boolean", meta = (DisplayName = "AND Boolean", CompactNodeTitle = "AND", Keywords = "& and"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_BooleanAND : public FVoxelTemplateNode_MultiInputBooleanNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetBooleanNode() const override
	{
		return FVoxelNode_BooleanAND::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_BooleanOR : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(bool, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(bool, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} || {B}";
	}
};

// Returns the logical OR of two values (A OR B)
USTRUCT(Category = "Math|Boolean", meta = (DisplayName = "OR Boolean", CompactNodeTitle = "OR", Keywords = "| or"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_BooleanOR : public FVoxelTemplateNode_MultiInputBooleanNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetBooleanNode() const override
	{
		return FVoxelNode_BooleanOR::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_BooleanNAND : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(bool, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(bool, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = !({A} && {B})";
	}
};

// Returns the logical NAND of two values NOT (A AND B)
USTRUCT(Category = "Math|Boolean", meta = (DisplayName = "NAND Boolean", CompactNodeTitle = "NAND", Keywords = "!& nand"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_BooleanNAND : public FVoxelTemplateNode_MultiInputBooleanNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetBooleanNode() const override
	{
		return FVoxelNode_BooleanNAND::StaticStruct();
	}
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Equal_Bool : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(bool, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(bool, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} == {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Equal_Byte : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(uint8, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(uint8, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} == {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Equal_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} == {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Equal_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} == {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Equal_Int : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} == {B}";
	}
};

// Returns true if A is exactly equal to B (A == B)
USTRUCT(Category = "Math|Operators", meta = (CompactNodeTitle = "==", Keywords = "== equal"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Equal : public FVoxelTemplateNode_EqualityBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetBoolInnerNode() const override
	{
		return FVoxelNode_Equal_Bool::StaticStruct();
	}
	virtual UScriptStruct* GetByteInnerNode() const override
	{
		return FVoxelNode_Equal_Byte::StaticStruct();
	}
	virtual UScriptStruct* GetFloatInnerNode() const override
	{
		return FVoxelNode_Equal_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleInnerNode() const override
	{
		return FVoxelNode_Equal_Double::StaticStruct();
	}
	virtual UScriptStruct* GetInt32InnerNode() const override
	{
		return FVoxelNode_Equal_Int::StaticStruct();
	}
	virtual UScriptStruct* GetObjectInnerNode() const override
	{
		return FVoxelNode_Equal_Object::StaticStruct();
	}
	virtual UScriptStruct* GetConnectionInnerNode() const override
	{
		return FVoxelNode_BooleanAND::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_NearlyEqual_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, B, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, ErrorTolerance, 1.e-6f);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = abs({A} - {B}) <= {ErrorTolerance}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_NearlyEqual_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, B, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, ErrorTolerance, 1.e-6f);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = abs({A} - {B}) <= {ErrorTolerance}";
	}
};

// Returns true if A is nearly equal to B (|A - B| < ErrorTolerance)
USTRUCT(Category = "Math|Operators", meta = (Keywords = "== equal"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_NearlyEqual : public FVoxelTemplateNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, B, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, ErrorTolerance, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, Result);

public:
	virtual FPin* ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const override;

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_NotEqual_Bool : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(bool, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(bool, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} != {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_NotEqual_Byte : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(uint8, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(uint8, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} != {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_NotEqual_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} != {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_NotEqual_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} != {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_NotEqual_Int : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} != {B}";
	}
};

// Returns true if A does not equal B (A != B)
USTRUCT(Category = "Math|Operators", meta = (CompactNodeTitle = "!=", Keywords = "!= not equal"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_NotEqual : public FVoxelTemplateNode_EqualityBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetBoolInnerNode() const override
	{
		return FVoxelNode_NotEqual_Bool::StaticStruct();
	}
	virtual UScriptStruct* GetByteInnerNode() const override
	{
		return FVoxelNode_NotEqual_Byte::StaticStruct();
	}
	virtual UScriptStruct* GetFloatInnerNode() const override
	{
		return FVoxelNode_NotEqual_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleInnerNode() const override
	{
		return FVoxelNode_NotEqual_Double::StaticStruct();
	}
	virtual UScriptStruct* GetInt32InnerNode() const override
	{
		return FVoxelNode_NotEqual_Int::StaticStruct();
	}
	virtual UScriptStruct* GetObjectInnerNode() const override
	{
		return FVoxelNode_NotEqual_Object::StaticStruct();
	}
	virtual UScriptStruct* GetConnectionInnerNode() const override
	{
		return FVoxelNode_BooleanOR::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Less_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} < {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Less_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} < {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Less_Int : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} < {B}";
	}
};

// Returns true if A is Less than B (A < B)
USTRUCT(Category = "Math|Operators", meta = (CompactNodeTitle = "<", Keywords = "< less"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Less : public FVoxelTemplateNode_EqualitySingleDimension
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetBoolInnerNode() const override
	{
		return nullptr;
	}
	virtual UScriptStruct* GetByteInnerNode() const override
	{
		return nullptr;
	}
	virtual UScriptStruct* GetFloatInnerNode() const override
	{
		return FVoxelNode_Less_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleInnerNode() const override
	{
		return FVoxelNode_Less_Double::StaticStruct();
	}
	virtual UScriptStruct* GetInt32InnerNode() const override
	{
		return FVoxelNode_Less_Int::StaticStruct();
	}
	virtual UScriptStruct* GetObjectInnerNode() const override
	{
		return nullptr;
	}
	virtual UScriptStruct* GetConnectionInnerNode() const override
	{
		return FVoxelNode_BooleanAND::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Greater_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} > {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Greater_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} > {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Greater_Int : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} > {B}";
	}
};

// Returns true if A is greater than B (A > B)
USTRUCT(Category = "Math|Operators", meta = (CompactNodeTitle = ">", Keywords = "> greater"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Greater : public FVoxelTemplateNode_EqualitySingleDimension
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetBoolInnerNode() const override
	{
		return nullptr;
	}
	virtual UScriptStruct* GetByteInnerNode() const override
	{
		return nullptr;
	}
	virtual UScriptStruct* GetFloatInnerNode() const override
	{
		return FVoxelNode_Greater_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleInnerNode() const override
	{
		return FVoxelNode_Greater_Double::StaticStruct();
	}
	virtual UScriptStruct* GetInt32InnerNode() const override
	{
		return FVoxelNode_Greater_Int::StaticStruct();
	}
	virtual UScriptStruct* GetObjectInnerNode() const override
	{
		return nullptr;
	}
	virtual UScriptStruct* GetConnectionInnerNode() const override
	{
		return FVoxelNode_BooleanAND::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_LessEqual_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} <= {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_LessEqual_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} <= {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_LessEqual_Int : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} <= {B}";
	}
};

// Returns true if A is less than or equal to B (A <= B)
USTRUCT(Category = "Math|Operators", meta = (CompactNodeTitle = "<=", Keywords = "<= less"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_LessEqual : public FVoxelTemplateNode_EqualitySingleDimension
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetBoolInnerNode() const override
	{
		return nullptr;
	}
	virtual UScriptStruct* GetByteInnerNode() const override
	{
		return nullptr;
	}
	virtual UScriptStruct* GetFloatInnerNode() const override
	{
		return FVoxelNode_LessEqual_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleInnerNode() const override
	{
		return FVoxelNode_LessEqual_Double::StaticStruct();
	}
	virtual UScriptStruct* GetInt32InnerNode() const override
	{
		return FVoxelNode_LessEqual_Int::StaticStruct();
	}
	virtual UScriptStruct* GetObjectInnerNode() const override
	{
		return nullptr;
	}
	virtual UScriptStruct* GetConnectionInnerNode() const override
	{
		return FVoxelNode_BooleanAND::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_GreaterEqual_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} >= {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_GreaterEqual_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} >= {B}";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_GreaterEqual_Int : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(bool, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {A} >= {B}";
	}
};

// Returns true if A is greater than or equal to B (A >= B)
USTRUCT(Category = "Math|Operators", meta = (CompactNodeTitle = ">=", Keywords = ">= greater"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_GreaterEqual : public FVoxelTemplateNode_EqualitySingleDimension
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetBoolInnerNode() const override
	{
		return nullptr;
	}
	virtual UScriptStruct* GetByteInnerNode() const override
	{
		return nullptr;
	}
	virtual UScriptStruct* GetFloatInnerNode() const override
	{
		return FVoxelNode_GreaterEqual_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleInnerNode() const override
	{
		return FVoxelNode_GreaterEqual_Double::StaticStruct();
	}
	virtual UScriptStruct* GetInt32InnerNode() const override
	{
		return FVoxelNode_GreaterEqual_Int::StaticStruct();
	}
	virtual UScriptStruct* GetObjectInnerNode() const override
	{
		return nullptr;
	}
	virtual UScriptStruct* GetConnectionInnerNode() const override
	{
		return FVoxelNode_BooleanAND::StaticStruct();
	}
};