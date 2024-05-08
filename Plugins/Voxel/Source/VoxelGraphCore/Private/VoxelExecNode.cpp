// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelExecNode.h"
#include "VoxelRuntime.h"
#include "VoxelExecNodeRuntimeWrapper.h"

DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelExecNodeRuntime);

DEFINE_VOXEL_NODE_COMPUTE(FVoxelExecNode, Exec)
{
	const TSharedRef<FVoxelQueryContext> Context = Query.GetSharedContext();
	const TSharedRef<FVoxelExecNode> Node = SharedNode<FVoxelExecNode>();

	FVoxelExec Exec;
	Exec.MakeRuntime = [=]
	{
		return Context->FindOrAddExecNodeRuntimeWrapper(Node);
	};
	return Exec;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<FVoxelExecNodeRuntime> FVoxelExecNode::CreateSharedExecRuntime(const TSharedRef<const FVoxelExecNode>& SharedThis) const
{
	TVoxelUniquePtr<FVoxelExecNodeRuntime> ExecRuntime = CreateExecRuntime(SharedThis);
	if (!ExecRuntime)
	{
		return nullptr;
	}

	return TSharedPtr<FVoxelExecNodeRuntime>(ExecRuntime.Release(), [](FVoxelExecNodeRuntime* Runtime)
	{
		if (!Runtime)
		{
			return;
		}

		FVoxelUtilities::RunOnGameThread([Runtime]
		{
			if (Runtime->bIsCreated)
			{
				Runtime->CallDestroy();
			}
			FVoxelMemory::Delete(Runtime);
		});
	});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelExecNodeRuntime::~FVoxelExecNodeRuntime()
{
	ensure(!bIsCreated || bIsDestroyed);
}

TSharedPtr<FVoxelRuntime> FVoxelExecNodeRuntime::GetRuntime() const
{
	const TSharedPtr<FVoxelRuntime> Runtime = PrivateContext->RuntimeInfo->GetRuntime();
	ensure(Runtime || IsDestroyed());
	return Runtime;
}

USceneComponent* FVoxelExecNodeRuntime::GetRootComponent() const
{
	ensure(IsInGameThread());
	USceneComponent* RootComponent = PrivateContext->RuntimeInfo->GetRootComponent();
	ensure(RootComponent);
	return RootComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelExecNodeRuntime::CallCreate(
	const TSharedRef<FVoxelQueryContext>& Context,
	TVoxelMap<FName, FVoxelRuntimePinValue>&& ConstantValues)
{
	VOXEL_FUNCTION_COUNTER();

	ensure(!bIsCreated);
	bIsCreated = true;

	PrivateContext = Context;
	PrivateConstantValues = MoveTemp(ConstantValues);

	const FVoxelQueryScope Scope(nullptr, &GetContext().Get());

	PreCreate();
	Create();
}

void FVoxelExecNodeRuntime::CallDestroy()
{
	VOXEL_FUNCTION_COUNTER();

	ensure(bIsCreated);
	ensure(!bIsDestroyed);
	bIsDestroyed = true;

	const FVoxelQueryScope Scope(nullptr, &GetContext().Get());

	Destroy();
}