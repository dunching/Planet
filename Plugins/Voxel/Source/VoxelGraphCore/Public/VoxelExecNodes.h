// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelExecNode.h"
#include "VoxelExecNodes.generated.h"

USTRUCT(Category = "Exec Nodes", meta = (NodeColor = "Red", NodeIconColor = "White", ShowInRootShortList))
struct VOXELGRAPHCORE_API FVoxelNode_Execute : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelExec, Exec, nullptr);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct VOXELGRAPHCORE_API FVoxelRootExecuteNode : public FVoxelExecNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelExec, ExecIn, nullptr, VirtualPin);

	virtual TVoxelUniquePtr<FVoxelExecNodeRuntime> CreateExecRuntime(const TSharedRef<const FVoxelExecNode>& SharedThis) const override;
};

class VOXELGRAPHCORE_API FVoxelRootExecuteNodeRuntime : public TVoxelExecNodeRuntime<FVoxelRootExecuteNode>
{
public:
	using Super::Super;

	//~ Begin FVoxelExecNodeRuntime Interface
	virtual void Create() override;
	virtual void Destroy() override;

	virtual void Tick(FVoxelRuntime& Runtime) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FVoxelOptionalBox GetBounds() const override;
	//~ End FVoxelExecNodeRuntime Interface

private:
	TVoxelDynamicValue<FVoxelExec> ExecValue;
	TSharedPtr<IVoxelExecNodeRuntimeInterface> NodeRuntime_GameThread;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(Category = "Exec Nodes")
struct VOXELGRAPHCORE_API FVoxelNode_MergeExecs : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN_ARRAY(FVoxelExec, Execs, nullptr, 1);
	VOXEL_OUTPUT_PIN(FVoxelExec, Exec);
};

class VOXELGRAPHCORE_API FVoxelMergeExecsNodeRuntime : public IVoxelExecNodeRuntimeInterface
{
public:
	const TVoxelArray<TSharedPtr<IVoxelExecNodeRuntimeInterface>> NodeRuntimes;

	explicit FVoxelMergeExecsNodeRuntime(const TVoxelArray<TSharedPtr<IVoxelExecNodeRuntimeInterface>>& NodeRuntimes)
		: NodeRuntimes(NodeRuntimes)
	{
	}

	//~ Begin IVoxelExecNodeRuntimeInterface Interface
	virtual void Tick(FVoxelRuntime& Runtime) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FVoxelOptionalBox GetBounds() const override;
	//~ End FVoxelExecNodeRuntime Interface
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(Category = "Exec Nodes|Debug")
struct VOXELGRAPHCORE_API FVoxelNode_DebugDelay : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN_ARRAY(FVoxelExec, Execs, nullptr, 1);
	VOXEL_INPUT_PIN(float, Delay, 1.f);
	VOXEL_OUTPUT_PIN(FVoxelExec, Exec);
};