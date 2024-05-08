// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelPointCollisionLargeChunk.h"
#include "VoxelRuntime.h"
#include "VoxelNodeMessages.h"
#include "VoxelBufferUtilities.h"
#include "VoxelFoliageSettings.h"
#include "VoxelInstancedCollisionComponent.h"

DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelPointCollisionLargeChunk);

FVoxelPointCollisionLargeChunk::FVoxelPointCollisionLargeChunk(
	const FVoxelGraphNodeRef& NodeRef,
	const FVoxelPointChunkRef& ChunkRef,
	const TVoxelDynamicValue<FVoxelPointSet>& PointsValue)
	: NodeRef(NodeRef)
	, ChunkRef(ChunkRef)
	, PointsValue(PointsValue)
{
}

void FVoxelPointCollisionLargeChunk::Initialize()
{
	PointsValue.OnChanged(MakeWeakPtrLambda(this, [this](const TSharedRef<const FVoxelPointSet>& NewPoints)
	{
		UpdatePoints(NewPoints);
	}));
}

void FVoxelPointCollisionLargeChunk::AddOnChanged(const FOnChanged& OnChanged)
{
	VOXEL_FUNCTION_COUNTER();

	TSharedPtr<const FMeshToPoints> MeshToPoints;
	{
		VOXEL_SCOPE_LOCK(CriticalSection);
		OnChanged_RequiresLock.Add(OnChanged);
		MeshToPoints = MeshToPoints_RequiresLock;
	}

	if (MeshToPoints)
	{
		OnChanged(*MeshToPoints);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelPointCollisionLargeChunk::UpdatePoints(const TSharedRef<const FVoxelPointSet>& NewPoints)
{
	VOXEL_SCOPE_COUNTER_FORMAT("FVoxelPointCollisionLargeChunk::UpdatePoints Num=%d", NewPoints->Num());
	ensure(!IsInGameThread());
	FVoxelNodeStatScope StatScope(*this, NewPoints->Num());

	FindVoxelPointSetAttributeVoid(*NewPoints, FVoxelPointAttributes::Id, FVoxelPointIdBuffer, NewIds);
	FindVoxelPointSetAttributeVoid(*NewPoints, FVoxelPointAttributes::Mesh, FVoxelStaticMeshBuffer, NewMeshes);
	FindVoxelPointSetAttributeVoid(*NewPoints, FVoxelPointAttributes::Position, FVoxelVectorBuffer, NewPositions);
	FindVoxelPointSetOptionalAttribute(*NewPoints, FVoxelPointAttributes::Rotation, FVoxelQuaternionBuffer, NewRotations, FQuat::Identity);
	FindVoxelPointSetOptionalAttribute(*NewPoints, FVoxelPointAttributes::Scale, FVoxelVectorBuffer, NewScales, FVector::OneVector);

	const TSharedRef<FMeshToPoints> MeshToPoints = MakeVoxelShared<FMeshToPoints>();

	ON_SCOPE_EXIT
	{
		TVoxelArray<FOnChanged> OnChangedCopy;
		{
			VOXEL_SCOPE_LOCK(CriticalSection);
			MeshToPoints_RequiresLock = MeshToPoints;
			OnChangedCopy = OnChanged_RequiresLock;
		}

		VOXEL_SCOPE_COUNTER("OnChanged");

		for (const FOnChanged& OnChanged : OnChangedCopy)
		{
			OnChanged(*MeshToPoints);
		}
	};

	for (int32 NewIndex = 0; NewIndex < NewPoints->Num(); NewIndex++)
	{
		const FVoxelStaticMesh Mesh = NewMeshes[NewIndex];

		TVoxelAddOnlyMap<FVoxelPointId, FPointTransform>& Points = MeshToPoints->FindOrAdd(Mesh);
		if (Points.Num() == 0)
		{
			VOXEL_SCOPE_COUNTER("Reserve");
			Points.Reserve(NewPoints->Num());
		}

		const FVoxelPointId PointId = NewIds[NewIndex];
		const uint32 Hash = Points.HashValue(PointId);

		if (Points.FindHashed(Hash, PointId))
		{
			VOXEL_MESSAGE(Error, "{0}: Duplicated point ids", this);
			continue;
		}

		FPointTransform& Transform = Points.AddHashed_CheckNew_NoRehash(Hash, PointId);

		Transform.PositionX = NewPositions.X[NewIndex];
		Transform.PositionY = NewPositions.Y[NewIndex];
		Transform.PositionZ = NewPositions.Z[NewIndex];

		Transform.ScaleX = NewScales.X[NewIndex];
		Transform.ScaleY = NewScales.Y[NewIndex];
		Transform.ScaleZ = NewScales.Z[NewIndex];

		Transform.RotationX = NewRotations.X[NewIndex];
		Transform.RotationY = NewRotations.Y[NewIndex];
		Transform.RotationZ = NewRotations.Z[NewIndex];
		Transform.RotationW = NewRotations.W[NewIndex];
	}
}