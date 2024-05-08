// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNameWrapper.generated.h"

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelNameWrapper
{
	GENERATED_BODY()

	FMinimalName Name;

	FVoxelNameWrapper() = default;
	FORCEINLINE FVoxelNameWrapper(const FName Name)
		: Name(Name)
	{
	}
	FORCEINLINE operator FName() const
	{
		return FName(Name);
	}

	FORCEINLINE bool operator==(const FVoxelNameWrapper& Other) const
	{
		return Name == Other.Name;
	}
	FORCEINLINE friend uint32 GetTypeHash(const FVoxelNameWrapper InSeed)
	{
		return GetTypeHash(InSeed.Name);
	}
};
checkStatic(sizeof(FVoxelNameWrapper) == 8);