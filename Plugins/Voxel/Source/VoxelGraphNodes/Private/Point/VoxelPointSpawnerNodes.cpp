// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Point/VoxelPointSpawnerNodes.h"
#include "VoxelBufferUtilities.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MakeRadialPointSpawner, PointSpawner)
{
	const TValue<FVoxelFloatRange> Radius = Get(RadiusPin, Query);
	const TValue<float> RadialOffset = Get(RadialOffsetPin, Query);
	const TValue<FVoxelInt32Range> NumPoints = Get(NumPointsPin, Query);
	const TValue<FVoxelSeed> Seed = Get(SeedPin, Query);

	return VOXEL_ON_COMPLETE(Radius, RadialOffset, NumPoints, Seed)
	{
		const float HalfRadialOffset = FMath::DegreesToRadians(RadialOffset) / 2;
		const FVoxelFloatRange RadialOffsetRange = FVoxelFloatRange(-HalfRadialOffset, HalfRadialOffset);

		const TSharedRef<FVoxelRadialPointSpawner> PointSpawner = MakeVoxelShared<FVoxelRadialPointSpawner>();
		PointSpawner->Node = GetNodeRef();
		PointSpawner->Radius = Radius;
		PointSpawner->RadialOffset = RadialOffsetRange;
		PointSpawner->NumPoints = NumPoints;
		PointSpawner->Seed = Seed;
		return PointSpawner;
	};
}

TSharedPtr<const FVoxelPointSet> FVoxelRadialPointSpawner::GeneratePoints(const FVoxelPointSet& Points) const
{
	VOXEL_FUNCTION_COUNTER();

	FindVoxelPointSetAttribute(Points, FVoxelPointAttributes::Id, FVoxelPointIdBuffer, ParentIds);
	FindVoxelPointSetAttribute(Points, FVoxelPointAttributes::Position, FVoxelVectorBuffer, ParentPositions);
	FindVoxelPointSetAttribute(Points, FVoxelPointAttributes::Normal, FVoxelVectorBuffer, ParentNormals);

	const FVoxelPointRandom ChildIdRandom(Seed, STATIC_HASH("RadialPointSpawner_ChildId"));
	const FVoxelPointRandom RadiusRandom(Seed, STATIC_HASH("RadialPointSpawner_Radius"));
	const FVoxelPointRandom RadialOffsetRandom(Seed, STATIC_HASH("RadialPointSpawner_RadialOffset"));
	const FVoxelPointRandom NumPointsRandom(Seed, STATIC_HASH("RadialPointSpawner_NumPoints"));

	TVoxelArray<int32> AllNumChildren;
	FVoxelUtilities::SetNumFast(AllNumChildren, Points.Num());

	int32 TotalNum = 0;
	{
		VOXEL_SCOPE_COUNTER("AllNumChildren");

		for (int32 ParentIndex = 0; ParentIndex < Points.Num(); ParentIndex++)
		{
			int32 NumChildren = NumPoints.Interpolate(NumPointsRandom.GetFraction(ParentIds[ParentIndex]));
			if (NumChildren > 1000)
			{
				VOXEL_MESSAGE(Error, "{0}: More than 1000 child points generated, skipping", this);
				NumChildren = 0;
			}
			NumChildren = FMath::Max(NumChildren, 0);

			AllNumChildren[ParentIndex] = NumChildren;
			TotalNum += NumChildren;
		}
	}

	if (TotalNum == 0)
	{
		return nullptr;
	}

	struct FParentInfo
	{
		int32 ParentIndex = 0;
		int32 ChildIndex = 0;
		int32 NumChildren = 0;
		FVector3f X = FVector3f(ForceInit);
		FVector3f Y = FVector3f(ForceInit);
	};
	TVoxelArray<FParentInfo> ParentInfos;
	FVoxelUtilities::SetNumFast(ParentInfos, TotalNum);
	{
		VOXEL_SCOPE_COUNTER("ParentInfos");

		int32 Index = 0;
		for (int32 ParentIndex = 0; ParentIndex < Points.Num(); ParentIndex++)
		{
			const int32 NumChildren = AllNumChildren[ParentIndex];
			if (NumChildren == 0)
			{
				continue;
			}

			const FVector3f Normal = ParentNormals[ParentIndex];

			FVector3f X = FMath::Abs(Normal.X) > 0.9f
				? FVector3f(0, 1, 0)
				: FVector3f(1, 0, 0);

			const FVector3f Y = FVector3f::CrossProduct(Normal, X).GetSafeNormal();
			X = FVector3f::CrossProduct(Y, Normal).GetSafeNormal();

			for (int32 ChildIndex = 0; ChildIndex < NumChildren; ChildIndex++)
			{
				ParentInfos[Index + ChildIndex] = FParentInfo
				{
					ParentIndex,
					ChildIndex,
					NumChildren,
					X,
					Y
				};
			}

			Index += NumChildren;
		}
		ensure(Index == TotalNum);
	}

	FVoxelPointIdBufferStorage Id;
	FVoxelFloatBufferStorage PositionX;
	FVoxelFloatBufferStorage PositionY;
	FVoxelFloatBufferStorage PositionZ;

	Id.Allocate(TotalNum);
	PositionX.Allocate(TotalNum);
	PositionY.Allocate(TotalNum);
	PositionZ.Allocate(TotalNum);

	for (int32 ChildIndex = 0; ChildIndex < TotalNum; ChildIndex++)
	{
		const FParentInfo ParentInfo = ParentInfos[ChildIndex];

		const FVoxelPointId ParentId = ParentIds[ParentInfo.ParentIndex];
		const FVector3f ParentPosition = ParentPositions[ParentInfo.ParentIndex];

		const FVoxelPointId ChildId = ChildIdRandom.MakeId(ParentId, ParentInfo.ChildIndex);

		float AngleInRadians = UE_TWO_PI * ParentInfo.ChildIndex / float(ParentInfo.NumChildren);
		AngleInRadians += RadialOffset.Interpolate(RadialOffsetRandom.GetFraction(ChildId));

		const float ChildRadius = Radius.Interpolate(RadiusRandom.GetFraction(ChildId));
		const FVector2f RelativePosition = FVector2f(FMath::Cos(AngleInRadians), FMath::Sin(AngleInRadians)) * ChildRadius;

		const FVector3f NewPosition =
			ParentPosition +
			ParentInfo.X * RelativePosition.X +
			ParentInfo.Y * RelativePosition.Y;

		Id[ChildIndex] = ChildId;
		PositionX[ChildIndex] = NewPosition.X;
		PositionY[ChildIndex] = NewPosition.Y;
		PositionZ[ChildIndex] = NewPosition.Z;
	}

	const TSharedRef<FVoxelPointSet> Result = MakeVoxelShared<FVoxelPointSet>();
	Result->SetNum(TotalNum);

	TVoxelMap<FName, TSharedPtr<const FVoxelBuffer>> NewBuffers;
	NewBuffers.Reserve(Points.GetAttributes().Num());

	for (const auto& It : Points.GetAttributes())
	{
		const TSharedRef<const FVoxelBuffer> Buffer = FVoxelBufferUtilities::Replicate(*It.Value, AllNumChildren, TotalNum);
		NewBuffers.Add(It.Key, Buffer);
	}

	// First add all existing buffers
	for (const auto& It : NewBuffers)
	{
		Result->Add(It.Key, It.Value.ToSharedRef());
	}

	// Then add parents, potentially overriding existing buffers
	// eg, we might have a new Parent Position replacing the previous one
	for (const auto& It : NewBuffers)
	{
		Result->Add(FVoxelPointAttributes::MakeParent(It.Key), It.Value.ToSharedRef());
	}

	Result->Add(FVoxelPointAttributes::Id, FVoxelPointIdBuffer::Make(Id));
	Result->Add(FVoxelPointAttributes::Position, FVoxelVectorBuffer::Make(PositionX, PositionY, PositionZ));
	return Result;
}