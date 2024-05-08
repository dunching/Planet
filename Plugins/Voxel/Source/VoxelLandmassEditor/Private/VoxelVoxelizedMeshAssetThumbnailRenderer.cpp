// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelVoxelizedMeshAssetThumbnailRenderer.h"
#include "VoxelVoxelizedMeshAsset.h"

DEFINE_VOXEL_THUMBNAIL_RENDERER(UVoxelVoxelizedMeshAssetThumbnailRenderer, UVoxelVoxelizedMeshAsset);

UStaticMesh* UVoxelVoxelizedMeshAssetThumbnailRenderer::GetStaticMesh(UObject* Object, TArray<UMaterialInterface*>& OutMaterialOverrides) const
{
	return CastChecked<UVoxelVoxelizedMeshAsset>(Object)->Mesh.LoadSynchronous();
}