// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelDynamicValue.h"
#include "Point/VoxelPointSet.h"
#include "Point/VoxelPointChunkRef.h"
#include "VoxelChunkedPointSet.generated.h"

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelChunkedPointSet
	: public FVoxelVirtualStruct
	, public TSharedFromThis<FVoxelChunkedPointSet>
	, public TVoxelRuntimeInfo<FVoxelChunkedPointSet>
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	FVoxelChunkedPointSet() = default;
	FVoxelChunkedPointSet(
		const int32 ChunkSize,
		const FVoxelPointChunkProviderRef& ChunkProviderRef,
		const TSharedRef<FVoxelQueryContext>& Context,
		const TSharedRef<const TVoxelComputeValue<FVoxelPointSet>>& ComputePoints);

	FORCEINLINE bool IsValid() const
	{
		return Context.IsValid();
	}
	FORCEINLINE const FVoxelRuntimeInfo& GetRuntimeInfoRef() const
	{
		return *Context->RuntimeInfo;
	}
	FORCEINLINE int32 GetChunkSize() const
	{
		return ChunkSize;
	}
	FORCEINLINE const FVoxelPointChunkProviderRef& GetChunkProviderRef() const
	{
		return ChunkProviderRef;
	}

	TVoxelFutureValue<FVoxelPointSet> GetPoints(
		FVoxelDependencyTracker& DependencyTracker,
		const FIntVector& ChunkMin) const;

	TVoxelDynamicValue<FVoxelPointSet> GetPointsValue(
		const FIntVector& ChunkMin,
		double PriorityOffset) const;

	void GetPointsInBounds(
		const FVoxelBox& Bounds,
		TFunction<void(const TSharedRef<const FVoxelPointSet>&)> Callback,
		TFunction<bool(const FVoxelIntBox& ChunkBounds)> ShouldComputeChunk = nullptr) const;

private:
	int32 ChunkSize = 0;
	FVoxelPointChunkProviderRef ChunkProviderRef;
	TSharedPtr<FVoxelQueryContext> Context;
	TSharedPtr<const TVoxelComputeValue<FVoxelPointSet>> ComputePoints;
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelPointChunkRefQueryParameter : public FVoxelQueryParameter
{
	GENERATED_BODY()
	GENERATED_VOXEL_QUERY_PARAMETER_BODY()

	FVoxelPointChunkRef ChunkRef;
};