// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Point/VoxelScatterPointsNode.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_ScatterPoints, Out)
{
	const TValue<FVoxelPointSet> Points = Get(InPin, Query);
	const TValue<FVoxelPointSpawner> PointSpawner = Get(PointSpawnerPin, Query);

	return VOXEL_ON_COMPLETE(Points, PointSpawner)
	{
		if (Points->Num() == 0)
		{
			return {};
		}
		if (PointSpawner->GetStruct() == FVoxelPointSpawner::StaticStruct())
		{
			VOXEL_MESSAGE(Error, "{0}: Invalid Point Spawner", this);
			return Points;
		}

		FVoxelNodeStatScope StatScope(*this, Points->Num());
		return PointSpawner->GeneratePoints(*Points);
	};
}