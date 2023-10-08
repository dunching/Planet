// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelBaseBuffers.h"
#include "VoxelRangeBuffers.generated.h"

DECLARE_VOXEL_BUFFER(FVoxelFloatRangeBuffer, FVoxelFloatRange);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelFloatRangeBuffer final : public FVoxelBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_BUFFER_BODY(FVoxelFloatRangeBuffer, FVoxelFloatRange);

	UPROPERTY()
	FVoxelFloatBuffer Min;

	UPROPERTY()
	FVoxelFloatBuffer Max;

	FORCEINLINE FVoxelFloatRange operator[](const int32 Index) const
	{
		return FVoxelFloatRange(Min[Index], Max[Index]);
	}

	static FVoxelFloatRangeBuffer Make(const FVoxelFloatRange& Value);
	static FVoxelFloatRangeBuffer Make(
		FVoxelFloatBufferStorage& InMin,
		FVoxelFloatBufferStorage& InMax);

	virtual void InitializeFromConstant(const FVoxelRuntimePinValue& Constant) override;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_BUFFER(FVoxelInt32RangeBuffer, FVoxelInt32Range);

USTRUCT(DisplayName = "Integer Range Buffer")
struct VOXELGRAPHCORE_API FVoxelInt32RangeBuffer final : public FVoxelBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_BUFFER_BODY(FVoxelInt32RangeBuffer, FVoxelInt32Range);

	UPROPERTY()
	FVoxelInt32Buffer Min;

	UPROPERTY()
	FVoxelInt32Buffer Max;

	FORCEINLINE FVoxelInt32Range operator[](const int32 Index) const
	{
		return FVoxelInt32Range(Min[Index], Max[Index]);
	}

	static FVoxelInt32RangeBuffer Make(const FVoxelInt32Range& Value);
	static FVoxelInt32RangeBuffer Make(
		FVoxelInt32BufferStorage& InMin,
		FVoxelInt32BufferStorage& InMax);

	virtual void InitializeFromConstant(const FVoxelRuntimePinValue& Constant) override;
};