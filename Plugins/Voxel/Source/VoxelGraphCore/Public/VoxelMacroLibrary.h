// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelGraph.h"
#include "VoxelMacroLibrary.generated.h"

UCLASS(meta = (VoxelAssetType, AssetColor = Blue))
class VOXELGRAPHCORE_API UVoxelMacroLibrary : public UObject
{
	GENERATED_BODY()

public:
	UVoxelMacroLibrary();

	UPROPERTY()
	TObjectPtr<UVoxelGraph> Graph;
};