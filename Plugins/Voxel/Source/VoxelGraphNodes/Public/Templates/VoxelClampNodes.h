// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelISPCNode.h"
#include "VoxelTemplateNode.h"
#include "VoxelClampNodes.generated.h"

USTRUCT(meta = (Abstract))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_AbstractClampBase : public FVoxelTemplateNode
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

public:
	virtual UScriptStruct* GetFloatInnerNode() const VOXEL_PURE_VIRTUAL({});
	virtual UScriptStruct* GetDoubleInnerNode() const VOXEL_PURE_VIRTUAL({});
	virtual UScriptStruct* GetInt32InnerNode() const VOXEL_PURE_VIRTUAL({});
};

USTRUCT(meta = (Abstract))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_ClampBase : public FVoxelTemplateNode_AbstractClampBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, Min, 0.f);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, Max, 1.f);
};

USTRUCT(meta = (Abstract))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_MappedClampBase : public FVoxelTemplateNode_AbstractClampBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, InMin, 0.f);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, InMax, 1.f);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, OutMin, 0.f);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, OutMax, 1.f);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Clamp_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, Min, 0.0f);
	VOXEL_TEMPLATE_INPUT_PIN(float, Max, 1.0f);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = clamp({Value}, {Min}, {Max})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Clamp_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, Value, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, Min, 0.0f);
	VOXEL_TEMPLATE_INPUT_PIN(double, Max, 1.0f);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = clamp({Value}, {Min}, {Max})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Clamp_Int : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(int32, Value, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, Min, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(int32, Max, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(int32, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = clamp({Value}, {Min}, {Max})";
	}
};

// Returns Value clamped to be between A and B
USTRUCT(Category = "Math|Misc")
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Clamp : public FVoxelTemplateNode_ClampBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetFloatInnerNode() const override
	{
		return FVoxelNode_Clamp_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleInnerNode() const override
	{
		return FVoxelNode_Clamp_Double::StaticStruct();
	}
	virtual UScriptStruct* GetInt32InnerNode() const override
	{
		return FVoxelNode_Clamp_Int::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_MappedRangeValueClamped_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, InMin, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, InMax, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, OutMin, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, OutMax, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = lerp({OutMin}, {OutMax}, clamp(({Value} - {InMin}) / ({InMax} - {InMin}), 0.f, 1.f))";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_MappedRangeValueClamped_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, Value, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, InMin, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, InMax, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, OutMin, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, OutMax, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = lerp({OutMin}, {OutMax}, clamp(({Value} - {InMin}) / ({InMax} - {InMin}), 0.d, 1.d))";
	}
};

// For the given Value clamped to the [InMin:InMax], returns the corresponding percentage in [OutMin:OutMax]
USTRUCT(Category = "Math|Misc")
struct VOXELGRAPHNODES_API FVoxelTemplateNode_MappedRangeValueClamped : public FVoxelTemplateNode_MappedClampBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetFloatInnerNode() const override
	{
		return FVoxelNode_MappedRangeValueClamped_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleInnerNode() const override
	{
		return FVoxelNode_MappedRangeValueClamped_Double::StaticStruct();
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
struct FVoxelNode_MappedRangeValueUnclamped_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(float, Value, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, InMin, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, InMax, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, OutMin, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, OutMax, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = lerp({OutMin}, {OutMax}, ({Value} - {InMin}) / ({InMax} - {InMin}))";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_MappedRangeValueUnclamped_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(double, Value, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, InMin, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, InMax, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, OutMin, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(double, OutMax, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = lerp({OutMin}, {OutMax}, ({Value} - {InMin}) / ({InMax} - {InMin}))";
	}
};

// Transform the given Value relative to the input range to the Output Range
USTRUCT(Category = "Math|Misc")
struct VOXELGRAPHNODES_API FVoxelTemplateNode_MappedRangeValueUnclamped : public FVoxelTemplateNode_MappedClampBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual UScriptStruct* GetFloatInnerNode() const override
	{
		return FVoxelNode_MappedRangeValueUnclamped_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleInnerNode() const override
	{
		return FVoxelNode_MappedRangeValueUnclamped_Double::StaticStruct();
	}
	virtual UScriptStruct* GetInt32InnerNode() const override
	{
		return nullptr;
	}
};