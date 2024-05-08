// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "Buffer/VoxelBaseBuffers.h"
#include "VoxelSelectNode.generated.h"

USTRUCT(Category = "Flow Control")
struct VOXELGRAPHNODES_API FVoxelNode_Select : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

public:
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, Index, nullptr, DisplayLast);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVoxelWildcard, Result);

	FVoxelNode_Select();

public:
	//~ Begin FVoxelNode Interface
#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif

	virtual void PreSerialize() override;
	virtual void PostSerialize() override;
	//~ End FVoxelNode Interface

public:
	TArray<FVoxelPinRef> ValuePins;

	UPROPERTY()
	int32 NumIntegerOptions = 2;

	UPROPERTY()
	FVoxelPinType SerializedIndexType;

	void FixupValuePins();

public:
#if WITH_EDITOR
	class FDefinition : public Super::FDefinition
	{
	public:
		GENERATED_VOXEL_NODE_DEFINITION_BODY(FVoxelNode_Select);

		virtual FString GetAddPinLabel() const override;
		virtual FString GetAddPinTooltip() const override;
		virtual FString GetRemovePinTooltip() const override;

		virtual bool CanAddInputPin() const override;
		virtual void AddInputPin() override;

		virtual bool CanRemoveInputPin() const override;
		virtual void RemoveInputPin() override;
	};
#endif
};