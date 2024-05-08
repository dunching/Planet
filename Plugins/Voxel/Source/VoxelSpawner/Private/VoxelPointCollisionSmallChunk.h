// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNodeHelpers.h"
#include "Point/VoxelChunkedPointSet.h"
#include "Buffer/VoxelStaticMeshBuffer.h"
#include "VoxelPointCollisionLargeChunk.h"

class FVoxelInstancedCollisionData;
class UVoxelInstancedCollisionComponent;

class FVoxelPointCollisionSmallChunk
	: public IVoxelNodeInterface
	, public TSharedFromThis<FVoxelPointCollisionSmallChunk>
	, public TVoxelRuntimeInfo<FVoxelPointCollisionSmallChunk>
{
public:
	const FVoxelGraphNodeRef NodeRef;
	const FVoxelPointChunkRef ChunkRef;
	const FVoxelBox Bounds;
	const TSharedRef<const FVoxelRuntimeInfo> RuntimeInfo;
	const TSharedRef<const FBodyInstance> BodyInstance;
	const TSharedRef<FVoxelPointCollisionLargeChunk> LargeChunk;

	VOXEL_COUNT_INSTANCES();

	FVoxelPointCollisionSmallChunk(
		const FVoxelGraphNodeRef& NodeRef,
		const FVoxelPointChunkRef& ChunkRef,
		const FVoxelBox& Bounds,
		const TSharedRef<const FVoxelRuntimeInfo>& RuntimeInfo,
		const TSharedRef<const FBodyInstance>& BodyInstance,
		const TSharedRef<FVoxelPointCollisionLargeChunk>& LargeChunk);

	FORCEINLINE const FVoxelRuntimeInfo& GetRuntimeInfoRef() const
	{
		return *RuntimeInfo;
	}

	void Initialize();
	void Destroy(FVoxelRuntime& Runtime);

	//~ Begin IVoxelNodeInterface Interface
	virtual const FVoxelGraphNodeRef& GetNodeRef() const override
	{
		return NodeRef;
	}
	//~ End IVoxelNodeInterface Interface

private:
	mutable FVoxelFastCriticalSection CriticalSection;

	struct FComponent
	{
		TWeakObjectPtr<UVoxelInstancedCollisionComponent> Component;
		TSharedPtr<FVoxelInstancedCollisionData> Data;
	};
	TVoxelMap<FVoxelStaticMesh, FComponent> MeshToComponent_RequiresLock;

	using FPointTransform = FVoxelPointCollisionLargeChunk::FPointTransform;
	using FMeshToPoints = FVoxelPointCollisionLargeChunk::FMeshToPoints;

	void UpdatePoints(const FMeshToPoints& MeshToPoints);
};