// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelTransformRef.h"
#include "VoxelBounds.generated.h"

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelBounds
{
	GENERATED_BODY()

public:
	FVoxelBounds() = default;

	FVoxelBounds(
		const FVoxelBox& Box,
		const FVoxelTransformRef& LocalToWorld)
		: bIsValid(true)
		, Box(Box)
		, LocalToWorld(LocalToWorld)
	{
	}

	static FVoxelBounds Infinite();

public:
	FORCEINLINE bool IsValid() const
	{
		return bIsValid;
	}
	FORCEINLINE bool IsInfinite() const
	{
		return Box.IsInfinite();
	}
	FORCEINLINE const FVoxelTransformRef& GetLocalToWorld() const
	{
		return LocalToWorld;
	}

public:
	FVoxelBounds ExtendLocal(float LocalAmount) const;

	FVoxelBounds Extend(
		float Amount,
		const FVoxelQuery& Query,
		const FVoxelTransformRef& AmountToWorld) const;

	float GetLocalValue(
		float Value,
		const FVoxelQuery& Query,
		const FVoxelTransformRef& ValueToWorld) const;

public:
	FVoxelBox GetBox_NoDependency(const FMatrix& OtherLocalToWorld) const;
	FVoxelBox GetBox_NoDependency(const FVoxelTransformRef& OtherLocalToWorld) const;
	FVoxelBox GetBox(const FVoxelQuery& Query, const FVoxelTransformRef& OtherLocalToWorld) const;

private:
	bool bIsValid = false;
	FVoxelBox Box;
	FVoxelTransformRef LocalToWorld;
};