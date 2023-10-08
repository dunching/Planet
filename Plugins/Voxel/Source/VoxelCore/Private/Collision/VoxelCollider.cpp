// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Collision/VoxelCollider.h"

DEFINE_VOXEL_MEMORY_STAT(STAT_VoxelColliderMemory);

int64 FVoxelCollider::GetAllocatedSize() const
{
	ensure(GetStruct() == StaticStruct());
	return 0;
}