// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelMaterialDefinitionThumbnailRenderer.generated.h"

UCLASS()
class UVoxelMaterialDefinitionThumbnailRenderer : public UVoxelStaticMeshThumbnailRenderer
{
	GENERATED_BODY()

public:
	//~ Begin UVoxelStaticMeshThumbnailRenderer Interface
	virtual UStaticMesh* GetStaticMesh(UObject* Object, TArray<UMaterialInterface*>& OutMaterialOverrides) const override;
	//~ End UVoxelStaticMeshThumbnailRenderer Interface
};