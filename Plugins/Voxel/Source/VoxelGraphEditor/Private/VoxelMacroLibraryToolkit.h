// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelMacroLibrary.h"
#include "Toolkits/VoxelSimpleAssetToolkit.h"
#include "VoxelMacroLibraryToolkit.generated.h"

USTRUCT()
struct FVoxelMacroLibraryToolkit : public FVoxelToolkit
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	UPROPERTY()
	TObjectPtr<UVoxelMacroLibrary> Asset;

public:
	//~ Begin FVoxelToolkit Interface
	virtual TArray<FMode> GetModes() const override;
	//~ End FVoxelToolkit Interface
};