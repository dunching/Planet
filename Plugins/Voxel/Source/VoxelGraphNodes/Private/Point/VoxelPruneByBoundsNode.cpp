// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Point/VoxelPruneByBoundsNode.h"
#include "Point/VoxelPointUtilities.h"
#include "VoxelAABBTree.h"
#include "Buffer/VoxelStaticMeshBuffer.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_PruneByBounds, Out)
{
	const TValue<FVoxelPointSet> PointsToPrune = Get(PointsToPrunePin, Query);
	const TValue<FVoxelPointSet> PointsToCheck = Get(PointsToCheckPin, Query);

	return VOXEL_ON_COMPLETE(PointsToPrune, PointsToCheck)
	{
		if (PointsToPrune->Num() == 0)
		{
			return {};
		}
		if (PointsToCheck->Num() == 0)
		{
			return PointsToPrune;
		}

		VOXEL_SCOPE_COUNTER_FORMAT("PruneByBounds PruneNum=%d CheckNum=%d", PointsToPrune->Num(), PointsToCheck->Num());
		FVoxelNodeStatScope StatScope(*this, PointsToPrune->Num());

		TVoxelArray<FVoxelAABBTree::FElement> Elements;
		{
			VOXEL_SCOPE_COUNTER_FORMAT("Build Elements Num=%d", PointsToCheck->Num());

			FindVoxelPointSetAttribute(*PointsToCheck, FVoxelPointAttributes::Mesh, FVoxelStaticMeshBuffer, MeshBuffer);
			FindVoxelPointSetAttribute(*PointsToCheck, FVoxelPointAttributes::Position, FVoxelVectorBuffer, PositionBuffer);
			FindVoxelPointSetOptionalAttribute(*PointsToCheck, FVoxelPointAttributes::Rotation, FVoxelQuaternionBuffer, RotationBuffer, FQuat::Identity);
			FindVoxelPointSetOptionalAttribute(*PointsToCheck, FVoxelPointAttributes::Scale, FVoxelVectorBuffer, ScaleBuffer, FVector::OneVector);

			FVoxelVectorBuffer BoundsMin;
			FVoxelVectorBuffer BoundsMax;
			FVoxelPointUtilities::GetMeshBounds(
				MeshBuffer,
				PositionBuffer,
				RotationBuffer,
				ScaleBuffer,
				BoundsMin,
				BoundsMax);

			FVoxelUtilities::SetNumFast(Elements, PointsToCheck->Num());
			for (int32 Index= 0; Index < PointsToCheck->Num(); Index++)
			{
				FVoxelAABBTree::FElement& Element = Elements[Index];

				Element.Bounds = FVoxelBox
				{
					FVector(BoundsMin[Index]),
					FVector(BoundsMax[Index]),
				};
				Element.Payload = Index;
			}
		}

		FVoxelAABBTree AABBTree;
		AABBTree.Initialize(MoveTemp(Elements));

		FindVoxelPointSetAttribute(*PointsToPrune, FVoxelPointAttributes::Mesh, FVoxelStaticMeshBuffer, MeshBuffer);
		FindVoxelPointSetAttribute(*PointsToPrune, FVoxelPointAttributes::Position, FVoxelVectorBuffer, PositionBuffer);
		FindVoxelPointSetOptionalAttribute(*PointsToPrune, FVoxelPointAttributes::Rotation, FVoxelQuaternionBuffer, RotationBuffer, FQuat::Identity);
		FindVoxelPointSetOptionalAttribute(*PointsToPrune, FVoxelPointAttributes::Scale, FVoxelVectorBuffer, ScaleBuffer, FVector::OneVector);

		FVoxelVectorBuffer BoundsMin;
		FVoxelVectorBuffer BoundsMax;
		FVoxelPointUtilities::GetMeshBounds(
			MeshBuffer,
			PositionBuffer,
			RotationBuffer,
			ScaleBuffer,
			BoundsMin,
			BoundsMax);

		FVoxelInt32BufferStorage Indices;
		Indices.Reserve(PointsToPrune->Num());

		VOXEL_SCOPE_COUNTER_FORMAT("Prune points Num=%d", PointsToPrune->Num());

		for (int32 Index = 0; Index < PointsToPrune->Num(); Index++)
		{
			const FVoxelBox Bounds
			{
				FVector(BoundsMin[Index]),
				FVector(BoundsMax[Index]),
			};

			if (!AABBTree.Overlap(Bounds))
			{
				Indices.Add(Index);
			}
		}

		return PointsToPrune->Gather(FVoxelInt32Buffer::Make(Indices));
	};
}