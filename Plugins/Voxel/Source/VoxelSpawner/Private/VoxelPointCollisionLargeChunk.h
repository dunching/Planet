// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNodeHelpers.h"
#include "Point/VoxelChunkedPointSet.h"
#include "Buffer/VoxelStaticMeshBuffer.h"

class FVoxelInstancedCollisionData;
class UVoxelInstancedCollisionComponent;

class FVoxelPointCollisionLargeChunk
	: public IVoxelNodeInterface
	, public TSharedFromThis<FVoxelPointCollisionLargeChunk>
	, public TVoxelRuntimeInfo<FVoxelPointCollisionLargeChunk>
{
public:
	const FVoxelGraphNodeRef NodeRef;
	const FVoxelPointChunkRef ChunkRef;
	const TVoxelDynamicValue<FVoxelPointSet> PointsValue;

	VOXEL_COUNT_INSTANCES();

	FVoxelPointCollisionLargeChunk(
		const FVoxelGraphNodeRef& NodeRef,
		const FVoxelPointChunkRef& ChunkRef,
		const TVoxelDynamicValue<FVoxelPointSet>& PointsValue);

	void Initialize();

public:
	struct FPointTransform
	{
		float PositionX;
		float PositionY;
		float PositionZ;
		float ScaleX;
		float ScaleY;
		float ScaleZ;
		float RotationX;
		float RotationY;
		float RotationZ;
		float RotationW;
	};

	using FMeshToPoints = TVoxelAddOnlyMap<FVoxelStaticMesh, TVoxelAddOnlyMap<FVoxelPointId, FPointTransform>>;
	using FOnChanged = TFunction<void(const FMeshToPoints& MeshToPoints)>;

	void AddOnChanged(const FOnChanged& OnChanged);

	//~ Begin IVoxelNodeInterface Interface
	virtual const FVoxelGraphNodeRef& GetNodeRef() const override
	{
		return NodeRef;
	}
	//~ End IVoxelNodeInterface Interface

private:
	mutable FVoxelFastCriticalSection CriticalSection;

	TSharedPtr<const FMeshToPoints> MeshToPoints_RequiresLock;
	TVoxelArray<FOnChanged> OnChanged_RequiresLock;

	void UpdatePoints(const TSharedRef<const FVoxelPointSet>& NewPoints);
};