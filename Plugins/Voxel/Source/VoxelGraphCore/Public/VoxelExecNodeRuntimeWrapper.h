// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelExecNode.h"

class VOXELGRAPHCORE_API FVoxelExecNodeRuntimeWrapper
	: public IVoxelExecNodeRuntimeInterface
	, public TSharedFromThis<FVoxelExecNodeRuntimeWrapper>
{
public:
	const TSharedRef<FVoxelExecNode> Node;

	explicit FVoxelExecNodeRuntimeWrapper(const TSharedRef<FVoxelExecNode>& Node)
		: Node(Node)
	{
	}

	void Initialize(const TSharedRef<FVoxelQueryContext>& NewContext);

	//~ Begin IVoxelExecNodeRuntimeInterface Interface
	virtual void Tick(FVoxelRuntime& Runtime) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FVoxelOptionalBox GetBounds() const override;
	//~ End IVoxelExecNodeRuntimeInterface Interface

private:
	TSharedPtr<FVoxelQueryContext> Context;
	TVoxelDynamicValue<bool> EnableNodeValue;

	struct FConstantValue : TSharedFromThis<FConstantValue>
	{
		FVoxelDynamicValue DynamicValue;
		FVoxelRuntimePinValue Value;
	};
	TMap<FName, TSharedPtr<FConstantValue>> ConstantPins_GameThread;

	TSharedPtr<FVoxelExecNodeRuntime> NodeRuntime_GameThread;

	void ComputeConstantPins();
	void OnConstantValueUpdated();
};