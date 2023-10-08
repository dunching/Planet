// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelObjectWithGuid.h"

void UVoxelObjectWithGuid::PostLoad()
{
	Super::PostLoad();

	if (IsTemplate())
	{
		return;
	}

	if (!Guid.IsValid())
	{
		Guid = FGuid::NewGuid();
	}
}

void UVoxelObjectWithGuid::PostInitProperties()
{
	Super::PostInitProperties();

	if (IsTemplate())
	{
		return;
	}

	if (!Guid.IsValid())
	{
		Guid = FGuid::NewGuid();
	}
}

void UVoxelObjectWithGuid::PostDuplicate(const EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);

	Guid = FGuid::NewGuid();
}