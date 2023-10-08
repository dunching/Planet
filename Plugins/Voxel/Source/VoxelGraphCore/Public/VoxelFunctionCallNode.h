// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelParameter.h"
#include "VoxelFunctionCallNode.generated.h"

class FVoxelInlineGraphData;

UENUM()
enum class EVoxelGraphMacroType : uint8
{
	// Regular macro
	Macro,
	// The graph being executed is exposed as a pin, letting you override it in instances
	Template,
	// Multiple graphs are executed sequentially, useful for clusters
	RecursiveTemplate
};

USTRUCT(meta = (Internal))
struct VOXELGRAPHCORE_API FVoxelNode_FunctionCall : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

public:
	UPROPERTY()
	TObjectPtr<UVoxelGraphInterface> GraphInterface;

	UPROPERTY()
	EVoxelGraphMacroType Type = {};

	//~ Begin FVoxelNode Interface
	virtual void PreCompile() override;
	virtual FVoxelComputeValue CompileCompute(FName PinName) const override;
	//~ End FVoxelNode Interface

	void FixupPins();
	UVoxelGraph* GetGraph() const;

private:
	TSharedPtr<FVoxelComputeInputMap> ComputeInputMap;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct VOXELGRAPHCORE_API FVoxelNode_FunctionCallInput_WithoutDefaultPin : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

public:
	UPROPERTY()
	FName Name;

	UPROPERTY()
	FVoxelPinValue DefaultValue;

	VOXEL_TEMPLATE_OUTPUT_PIN(FVoxelWildcard, Value);

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override
	{
		return FVoxelPinTypeSet::All();
	}
#endif

	//~ Begin FVoxelNode Interface
	virtual void PreCompile() override;
	//~ End FVoxelNode Interface

public:
	FVoxelRuntimePinValue RuntimeDefaultValue;
};

USTRUCT(meta = (Internal))
struct VOXELGRAPHCORE_API FVoxelNode_FunctionCallInput_WithDefaultPin : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	UPROPERTY()
	FName Name;

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, Default, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVoxelWildcard, Value);

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override
	{
		return FVoxelPinTypeSet::All();
	}
#endif
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct VOXELGRAPHCORE_API FVoxelNode_FunctionCallOutput : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	UPROPERTY()
	FName Name;

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, Value, nullptr);

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override
	{
		return FVoxelPinTypeSet::All();
	}
#endif
};