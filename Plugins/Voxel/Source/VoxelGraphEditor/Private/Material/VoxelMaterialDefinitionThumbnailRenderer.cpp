// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMaterialDefinitionThumbnailRenderer.h"
#include "Material/VoxelMaterialDefinitionInterface.h"

DEFINE_VOXEL_THUMBNAIL_RENDERER(UVoxelMaterialDefinitionThumbnailRenderer, UVoxelMaterialDefinitionInterface);

UStaticMesh* UVoxelMaterialDefinitionThumbnailRenderer::GetStaticMesh(UObject* Object, TArray<UMaterialInterface*>& OutMaterialOverrides) const
{
	OutMaterialOverrides.Add(CastChecked<UVoxelMaterialDefinitionInterface>(Object)->GetPreviewMaterial());

	static TWeakObjectPtr<UStaticMesh> PreviewMesh;
	if (!PreviewMesh.IsValid())
	{
		PreviewMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/EditorMeshes/EditorSphere.EditorSphere"));
	}

	ensure(PreviewMesh.IsValid());
	return PreviewMesh.Get();
}