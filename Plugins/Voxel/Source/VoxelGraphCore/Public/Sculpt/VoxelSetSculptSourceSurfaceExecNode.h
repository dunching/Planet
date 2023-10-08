// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelSurface.h"
#include "VoxelExecNode.h"
#include "VoxelSetSculptSourceSurfaceExecNode.generated.h"

// Set the surface that the sculpt data should default to
// ie, the surface that should be used if no edits have been made
USTRUCT(DisplayName = "Set Sculpt Source Surface")
struct VOXELGRAPHCORE_API FVoxelSetSculptSourceSurfaceExecNode : public FVoxelExecNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelSurface, Surface, nullptr, VirtualPin);
	VOXEL_INPUT_PIN(float, VoxelSize, 100.f, ConstantPin);

	virtual TVoxelUniquePtr<FVoxelExecNodeRuntime> CreateExecRuntime(const TSharedRef<const FVoxelExecNode>& SharedThis) const override;
};

class VOXELGRAPHCORE_API FVoxelSetSculptSourceSurfaceExecNodeRuntime : public TVoxelExecNodeRuntime<FVoxelSetSculptSourceSurfaceExecNode>
{
public:
	using Super::Super;

	//~ Begin FVoxelExecNodeRuntime Interface
	virtual void Create() override;
	virtual void Destroy() override;
	//~ End FVoxelExecNodeRuntime Interface
};