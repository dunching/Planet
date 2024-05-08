// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelExecNode.h"
#include "VoxelFoliageSettings.h"
#include "Point/VoxelChunkedPointSet.h"
#include "VoxelRenderMeshNode.generated.h"

class FVoxelInvokerView;
class FVoxelRenderMeshChunk;

USTRUCT(DisplayName = "Render Mesh")
struct VOXELSPAWNER_API FVoxelRenderMeshExecNode : public FVoxelExecNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelChunkedPointSet, ChunkedPoints, nullptr, ConstantPin);
	VOXEL_INPUT_PIN(float, RenderDistance, 10000.f, ConstantPin);
	VOXEL_INPUT_PIN(float, MinRenderDistance, 0.f, ConstantPin, AdvancedDisplay);
	VOXEL_INPUT_PIN(float, FadeDistance, 1000.f, ConstantPin, AdvancedDisplay);
	VOXEL_INPUT_PIN(FVoxelFoliageSettings, FoliageSettings, nullptr, ConstantPin, AdvancedDisplay);
	// Priority offset, added to the task distance from camera
	// Closest tasks are computed first, so set this to a very low value (eg, -1000000) if you want it to be computed first
	VOXEL_INPUT_PIN(double, PriorityOffset, 0, ConstantPin, AdvancedDisplay);

	virtual TVoxelUniquePtr<FVoxelExecNodeRuntime> CreateExecRuntime(const TSharedRef<const FVoxelExecNode>& SharedThis) const override;
};

class VOXELSPAWNER_API FVoxelRenderMeshExecNodeRuntime : public TVoxelExecNodeRuntime<FVoxelRenderMeshExecNode>
{
public:
	using Super::Super;

	//~ Begin FVoxelExecNodeRuntime Interface
	virtual void Create() override;
	virtual void Destroy() override;
	virtual FVoxelOptionalBox GetBounds() const override;
	//~ End FVoxelExecNodeRuntime Interface

private:
	TSharedPtr<FVoxelInvokerView> InvokerView;

	mutable FVoxelFastCriticalSection CriticalSection;
	TVoxelMap<FIntVector, TSharedPtr<FVoxelRenderMeshChunk>> Chunks_RequiresLock;
};