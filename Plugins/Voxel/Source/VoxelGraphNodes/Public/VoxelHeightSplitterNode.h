// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "Buffer/VoxelBaseBuffers.h"
#include "VoxelHeightSplitterNode.generated.h"

USTRUCT(Category = "Math|Misc")
struct VOXELGRAPHNODES_API FVoxelNode_HeightSplitter : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	FVoxelNode_HeightSplitter();

	VOXEL_INPUT_PIN(FVoxelFloatBuffer, Height, nullptr);

	virtual FVoxelComputeValue CompileCompute(FName PinName) const override;

	virtual void PostSerialize() override;

public:
	struct FLayerPin
	{
		TVoxelPinRef<float> Height;
		TVoxelPinRef<float> Falloff;
	};
	TArray<FLayerPin> LayerPins;
	TArray<TVoxelPinRef<FVoxelFloatBuffer>> ResultPins;

	UPROPERTY()
	int32 NumLayerPins = 1;

	void FixupLayerPins();

public:
#if WITH_EDITOR
	class FDefinition : public Super::FDefinition
	{
	public:
		GENERATED_VOXEL_NODE_DEFINITION_BODY(FVoxelNode_HeightSplitter);

		virtual FString GetAddPinLabel() const override;
		virtual FString GetAddPinTooltip() const override;

		virtual bool CanAddInputPin() const override { return true; }
		virtual void AddInputPin() override;

		virtual bool CanRemoveInputPin() const override;
		virtual void RemoveInputPin() override;
	};
#endif
};