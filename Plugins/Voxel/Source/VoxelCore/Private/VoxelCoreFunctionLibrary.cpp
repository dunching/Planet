// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelCoreFunctionLibrary.h"

void UVoxelCoreFunctionLibrary::RefreshMaterialInstance(UMaterialInstanceDynamic* MaterialInstance)
{
	VOXEL_FUNCTION_COUNTER();

	if (!MaterialInstance)
	{
		VOXEL_MESSAGE(Error, "MaterialInstance is null");
		return;
	}

	FVoxelMaterialRef::RefreshInstance(*MaterialInstance);
}