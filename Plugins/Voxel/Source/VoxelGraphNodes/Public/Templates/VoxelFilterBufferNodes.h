// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelTemplateNode.h"
#include "Buffer/VoxelBaseBuffers.h"
#include "VoxelFilterBufferNodes.generated.h"

USTRUCT(meta = (Internal))
struct VOXELGRAPHNODES_API FVoxelNode_FilterBuffer : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelWildcardBuffer, Value, nullptr);
	VOXEL_INPUT_PIN(FVoxelBoolBuffer, Condition, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelWildcardBuffer, OutValue);

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif
};

USTRUCT(Category = "Misc")
struct VOXELGRAPHNODES_API FVoxelTemplateNode_FilterBuffer : public FVoxelTemplateNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelBoolBuffer, Condition, nullptr, DisplayLast);

public:
	FVoxelTemplateNode_FilterBuffer();

	virtual void ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins, TArray<FPin*>& OutPins) const override;

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif

	virtual void PostSerialize() override;

public:
	TArray<FVoxelPinRef> BufferInputPins;
	TArray<FVoxelPinRef> BufferOutputPins;

	UPROPERTY()
	int32 NumBufferPins = 1;

	void FixupBufferPins();

public:
#if WITH_EDITOR
	class FDefinition : public Super::FDefinition
	{
	public:
		GENERATED_VOXEL_NODE_DEFINITION_BODY(FVoxelTemplateNode_FilterBuffer);

		virtual FString GetAddPinLabel() const override;
		virtual FString GetAddPinTooltip() const override;

		virtual bool CanAddInputPin() const override { return true; }
		virtual void AddInputPin() override;

		virtual bool CanRemoveInputPin() const override;
		virtual void RemoveInputPin() override;
	};
#endif
};