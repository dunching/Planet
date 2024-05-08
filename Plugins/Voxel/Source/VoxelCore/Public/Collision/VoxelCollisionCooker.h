// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"

struct FVoxelTriangleMeshCollider;

struct VOXELCORE_API FVoxelCollisionCooker
{
	static TSharedPtr<FVoxelTriangleMeshCollider> CookTriangleMesh(
		TConstVoxelArrayView<int32> Indices,
		TConstVoxelArrayView<FVector3f> Vertices,
		TConstVoxelArrayView<uint16> FaceMaterials);
};