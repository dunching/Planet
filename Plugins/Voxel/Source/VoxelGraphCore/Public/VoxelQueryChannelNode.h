// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelChannel.h"
#include "VoxelExecNode.h"
#include "VoxelQueryChannelNode.generated.h"

USTRUCT()
struct FVoxelBrushPriorityQueryParameter : public FVoxelQueryParameter
{
	GENERATED_BODY()
	GENERATED_VOXEL_QUERY_PARAMETER_BODY()

	TVoxelMap<FName, FVoxelBrushPriority> ChannelToPriority;

	static void AddMaxPriority(
		FVoxelQueryParameters& Parameters,
		FName ChannelName,
		FVoxelBrushPriority Priority);
};

USTRUCT(Category = "Channel Nodes")
struct VOXELGRAPHCORE_API FVoxelNode_QueryChannel : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelChannelName, Channel, nullptr);
	// Only brushes with a priority strictly inferior to this will be sampled
	// Useful to sample a channel before the current graph affects it when drawing a preview mesh
	VOXEL_INPUT_PIN(int32, MaxPriority, MAX_int16, AdvancedDisplay);
	VOXEL_OUTPUT_PIN(FVoxelWildcard, Value);

	FVoxelNode_QueryChannel();

#if WITH_EDITOR
	class FDefinition : public Super::FDefinition
	{
	public:
		GENERATED_VOXEL_NODE_DEFINITION_BODY(FVoxelNode_QueryChannel);

		virtual void Initialize(UEdGraphNode& EdGraphNode) override;
		virtual bool OnPinDefaultValueChanged(FName PinName, const FVoxelPinValue& NewDefaultValue) override;
	};
#endif
};