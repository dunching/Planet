// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelAppendNamesNode.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_AppendNames, Name)
{
	const TVoxelArray<TValue<FName>> Names = Get(NamesPins, Query);

	return VOXEL_ON_COMPLETE(Names)
	{
		TStringBuilderWithBuffer<TCHAR, NAME_SIZE> String;
		for (const FName Name : Names)
		{
			Name.AppendString(String);
		}
		return FName(String);
	};
}