// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelSurface.h"
#include "VoxelExecNode.h"
#include "VoxelEditSculptSurfaceExecNode.generated.h"

// Used in sculpt tools
USTRUCT(DisplayName = "Edit Sculpt Surface")
struct VOXELGRAPHCORE_API FVoxelEditSculptSurfaceExecNode : public FVoxelExecNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelSurface, NewSurface, nullptr, VirtualPin);
	VOXEL_INPUT_PIN(FVoxelBounds, Bounds, nullptr, VirtualPin);

	virtual TVoxelUniquePtr<FVoxelExecNodeRuntime> CreateExecRuntime(const TSharedRef<const FVoxelExecNode>& SharedThis) const override;
};

class VOXELGRAPHCORE_API FVoxelEditSculptSurfaceExecNodeRuntime : public TVoxelExecNodeRuntime<FVoxelEditSculptSurfaceExecNode>
{
public:
	using Super::Super;

	//~ Begin FVoxelExecNodeRuntime Interface
	virtual void Create() override;
	virtual void Destroy() override;
	//~ End FVoxelExecNodeRuntime Interface
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelRuntimeParameter_EditSculptSurface : public FVoxelRuntimeParameter
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	mutable FVoxelFastCriticalSection CriticalSection;
	mutable TWeakPtr<FVoxelEditSculptSurfaceExecNodeRuntime> WeakRuntime;
};