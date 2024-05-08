// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelMeshVoxelizerLibrary.h"

class UVoxelVoxelizedMeshAsset;

DECLARE_VOXEL_MEMORY_STAT(VOXELLANDMASS_API, STAT_VoxelVoxelizedMeshData, "Voxel Voxelized Mesh Data Memory");

class VOXELLANDMASS_API FVoxelVoxelizedMeshData
{
public:
	FVoxelBox MeshBounds;
	float VoxelSize = 0;
	float MaxSmoothness = 0;
	FVoxelMeshVoxelizerSettings VoxelizerSettings;

	FVector3f Origin = FVector3f::ZeroVector;
	FIntVector Size = FIntVector::ZeroValue;
	TVoxelArray<float> DistanceField;
	TVoxelArray<FVoxelOctahedron> Normals;

	FVoxelVoxelizedMeshData() = default;

	VOXEL_ALLOCATED_SIZE_TRACKER(STAT_VoxelVoxelizedMeshData);

	int64 GetAllocatedSize() const;
	void Serialize(FArchive& Ar);

	static TSharedPtr<FVoxelVoxelizedMeshData> VoxelizeMesh(
		const UStaticMesh& Mesh,
		float VoxelSize,
		float MaxSmoothness,
		const FVoxelMeshVoxelizerSettings& VoxelizerSettings);
};