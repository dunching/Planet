// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelExecNode.h"
#include "Point/VoxelChunkedPointSet.h"
#include "VoxelPointCollisionNode.generated.h"

class FVoxelInvokerView;
class FVoxelPointCollisionLargeChunk;
class FVoxelPointCollisionSmallChunk;

USTRUCT(DisplayName = "Generate Point Collision")
struct VOXELSPAWNER_API FVoxelPointCollisionExecNode : public FVoxelExecNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelChunkedPointSet, ChunkedPoints, nullptr, ConstantPin);
	VOXEL_INPUT_PIN(FBodyInstance, BodyInstance, nullptr, ConstantPin);
	VOXEL_INPUT_PIN(FName, InvokerChannel, "Default", ConstantPin);
	// Will be added to invoker radius
	VOXEL_INPUT_PIN(float, DistanceOffset, 0.f, ConstantPin);
	// In cm, granularity of collision
	// Try to keep high enough to not have too many chunks
	VOXEL_INPUT_PIN(int32, ChunkSize, 1000, ConstantPin);
	// Priority offset, added to the task distance from camera
	// Closest tasks are computed first, so set this to a very low value (eg, -1000000) if you want it to be computed first
	VOXEL_INPUT_PIN(double, PriorityOffset, -1000000, ConstantPin, AdvancedDisplay);

	virtual TVoxelUniquePtr<FVoxelExecNodeRuntime> CreateExecRuntime(const TSharedRef<const FVoxelExecNode>& SharedThis) const override;
};

class VOXELSPAWNER_API FVoxelPointCollisionExecNodeRuntime : public TVoxelExecNodeRuntime<FVoxelPointCollisionExecNode>
{
public:
	using Super::Super;

	//~ Begin FVoxelExecNodeRuntime Interface
	virtual void Create() override;
	virtual void Destroy() override;
	//~ End FVoxelExecNodeRuntime Interface

private:
	TSharedPtr<FVoxelInvokerView> InvokerView;

	mutable FVoxelFastCriticalSection CriticalSection;
	TVoxelMap<FIntVector, TWeakPtr<FVoxelPointCollisionLargeChunk>> LargeChunks_RequiresLock;
	TVoxelMap<FIntVector, TSharedPtr<FVoxelPointCollisionSmallChunk>> SmallChunks_RequiresLock;
};