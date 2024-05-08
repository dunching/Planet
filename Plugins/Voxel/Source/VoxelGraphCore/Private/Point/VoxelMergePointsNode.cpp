// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Point/VoxelMergePointsNode.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MergePoints, Out)
{
	const TVoxelArray<TValue<FVoxelPointSet>> Inputs = Get(InputPins, Query);

	return VOXEL_ON_COMPLETE(Inputs)
	{
		int32 Num = 0;
		for (const TSharedRef<const FVoxelPointSet>& Input : Inputs)
		{
			Num += Input->Num();
		}
		FVoxelNodeStatScope StatScope(*this, Num);

		return FVoxelPointSet::Merge(Inputs);
	};
}