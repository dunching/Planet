// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelParameter.h"
#include "VoxelParameterOverrideCollection_DEPRECATED.generated.h"

class UVoxelParameterContainer;

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelParameterOverride_DEPRECATED
{
	GENERATED_BODY()

	UPROPERTY()
	bool bEnable = false;

	UPROPERTY()
	FVoxelParameter Parameter;

	UPROPERTY()
	FVoxelPinValue ValueOverride;

	UPROPERTY()
	bool bIsOrphan = false;
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelParameterOverrideCollection_DEPRECATED
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Voxel")
	TArray<FVoxelParameterOverride_DEPRECATED> Parameters;

	UPROPERTY(EditAnywhere, Category = "Voxel")
	TArray<FName> Categories;

	void MigrateTo(UVoxelParameterContainer& ParameterContainer);
};