// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Sculpt/VoxelEditSculptSurfaceExecNode.h"

TVoxelUniquePtr<FVoxelExecNodeRuntime> FVoxelEditSculptSurfaceExecNode::CreateExecRuntime(const TSharedRef<const FVoxelExecNode>& SharedThis) const
{
	return MakeVoxelUnique<FVoxelEditSculptSurfaceExecNodeRuntime>(SharedThis);
}

void FVoxelEditSculptSurfaceExecNodeRuntime::Create()
{
	Super::Create();

	const TSharedPtr<const FVoxelRuntimeParameter_EditSculptSurface> Parameter = FindParameter<FVoxelRuntimeParameter_EditSculptSurface>();
	if (!Parameter)
	{
		return;
	}

	VOXEL_SCOPE_LOCK(Parameter->CriticalSection);

	if (const TSharedPtr<FVoxelEditSculptSurfaceExecNodeRuntime> Runtime = Parameter->WeakRuntime.Pin())
	{
		VOXEL_MESSAGE(Error, "Multiple Edit Sculpt Surface nodes: {0}, {1}", this, Runtime.Get());
		return;
	}

	Parameter->WeakRuntime = SharedThis(this);
}

void FVoxelEditSculptSurfaceExecNodeRuntime::Destroy()
{
	Super::Destroy();

	const TSharedPtr<const FVoxelRuntimeParameter_EditSculptSurface> Parameter = FindParameter<FVoxelRuntimeParameter_EditSculptSurface>();
	if (!Parameter)
	{
		return;
	}

	VOXEL_SCOPE_LOCK(Parameter->CriticalSection);

	if (GetWeakPtrObject_Unsafe(Parameter->WeakRuntime) == this)
	{
		Parameter->WeakRuntime = {};
	}
}