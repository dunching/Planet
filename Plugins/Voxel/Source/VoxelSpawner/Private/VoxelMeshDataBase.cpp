// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMeshDataBase.h"
#include "VoxelAABBTree.h"

int64 FVoxelMeshDataBase::GetAllocatedSize() const
{
	int64 AllocatedSize = 0;
	AllocatedSize += Transforms.GetAllocatedSize();
	AllocatedSize += PointIds_Transient.GetAllocatedSize();
	AllocatedSize += CustomDatas_Transient.GetAllocatedSize();

	for (const TVoxelArray<float>& CustomData : CustomDatas_Transient)
	{
		AllocatedSize += CustomData.GetAllocatedSize();
	}

	return AllocatedSize;
}