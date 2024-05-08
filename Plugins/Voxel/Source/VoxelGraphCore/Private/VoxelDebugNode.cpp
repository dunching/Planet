// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelDebugNode.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelDebugNode, Out)
{
	if (const FVoxelDebugQueryParameter* DebugQueryParameter = Query.GetParameters().Find<FVoxelDebugQueryParameter>())
	{
		VOXEL_SCOPE_LOCK(DebugQueryParameter->CriticalSection);

		DebugQueryParameter->Entries_RequiresLock.Add(
			GetNodeRef(),
			FVoxelDebugQueryParameter::FEntry
			{
				GetNodeRef(),
					ReturnPinType,
					GetCompute(InPin, Query.GetSharedContext())
			});
	}

	return Get(InPin, Query);
}