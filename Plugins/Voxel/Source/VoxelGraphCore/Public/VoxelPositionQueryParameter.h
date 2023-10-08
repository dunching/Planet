// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelQuery.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "VoxelPositionQueryParameter.generated.h"

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelPositionQueryParameter : public FVoxelQueryParameter
{
	GENERATED_BODY()
	GENERATED_VOXEL_QUERY_PARAMETER_BODY()

public:
	bool IsGradient() const
	{
		return bIsGradient;
	}
	bool IsGrid() const
	{
		return Grid.IsValid();
	}

	struct FGrid
	{
		FVector3f Start;
		float Step = 0.f;
		FIntVector Size;
	};
	const FGrid& GetGrid() const
	{
		return *Grid;
	}

	FVoxelBox GetBounds() const;
	FVoxelVectorBuffer GetPositions() const;

	void Initialize(
		const FVoxelVectorBuffer& NewPositions,
		const TOptional<FVoxelBox>& NewBounds = {});
	void Initialize(
		TVoxelUniqueFunction<FVoxelVectorBuffer()>&& NewCompute,
		const TOptional<FVoxelBox>& NewBounds = {});
	void InitializeGradient(
		const FVoxelVectorBuffer& NewPositions,
		const TOptional<FVoxelBox>& NewBounds = {});

	void InitializeGrid(
		const FVector3f& Start,
		float Step,
		const FIntVector& Size);

public:
	static FVoxelQuery TransformQuery(
		const FVoxelQuery& Query,
		const FMatrix& Transform);

private:
	TSharedPtr<const FGrid> Grid;
	bool bIsGradient = false;
	TOptional<FVoxelBox> PrecomputedBounds;
	TSharedPtr<const TVoxelUniqueFunction<FVoxelVectorBuffer()>> Compute;

	mutable FVoxelFastCriticalSection CriticalSection;
	mutable TOptional<FVoxelBox> CachedBounds_RequiresLock;
	mutable TOptional<FVoxelVectorBuffer> CachedPositions_RequiresLock;

	void CheckBounds() const;
};