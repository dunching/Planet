// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelBaseBuffers.h"
#include "VoxelIntegerBuffers.generated.h"

DECLARE_VOXEL_BUFFER(FVoxelIntPointBuffer, FIntPoint);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelIntPointBuffer final : public FVoxelBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_BUFFER_BODY(FVoxelIntPointBuffer, FIntPoint);

	UPROPERTY()
	FVoxelInt32Buffer X;

	UPROPERTY()
	FVoxelInt32Buffer Y;

	FORCEINLINE FIntPoint operator[](const int32 Index) const
	{
		return FIntPoint(X[Index], Y[Index]);
	}

	static FVoxelIntPointBuffer Make(const FIntPoint& Value);
	static FVoxelIntPointBuffer Make(
		FVoxelInt32BufferStorage& InX,
		FVoxelInt32BufferStorage& InY);

	virtual void InitializeFromConstant(const FVoxelRuntimePinValue& Constant) override;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_BUFFER(FVoxelIntVectorBuffer, FIntVector);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelIntVectorBuffer final : public FVoxelBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_BUFFER_BODY(FVoxelIntVectorBuffer, FIntVector);

	UPROPERTY()
	FVoxelInt32Buffer X;

	UPROPERTY()
	FVoxelInt32Buffer Y;

	UPROPERTY()
	FVoxelInt32Buffer Z;

	FORCEINLINE FIntVector operator[](const int32 Index) const
	{
		return FIntVector(X[Index], Y[Index], Z[Index]);
	}

	static FVoxelIntVectorBuffer Make(const FIntVector& Value);
	static FVoxelIntVectorBuffer Make(
		FVoxelInt32BufferStorage& InX,
		FVoxelInt32BufferStorage& InY,
		FVoxelInt32BufferStorage& InZ);

	virtual void InitializeFromConstant(const FVoxelRuntimePinValue& Constant) override;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_BUFFER(FVoxelIntVector4Buffer, FIntVector4);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelIntVector4Buffer final : public FVoxelBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_BUFFER_BODY(FVoxelIntVector4Buffer, FIntVector4);

	UPROPERTY()
	FVoxelInt32Buffer X;

	UPROPERTY()
	FVoxelInt32Buffer Y;

	UPROPERTY()
	FVoxelInt32Buffer Z;

	UPROPERTY()
	FVoxelInt32Buffer W;

	FORCEINLINE FIntVector4 operator[](const int32 Index) const
	{
		return FIntVector4(X[Index], Y[Index], Z[Index], W[Index]);
	}

	static FVoxelIntVector4Buffer Make(const FIntVector4& Value);
	static FVoxelIntVector4Buffer Make(
		FVoxelInt32BufferStorage& InX,
		FVoxelInt32BufferStorage& InY,
		FVoxelInt32BufferStorage& InZ,
		FVoxelInt32BufferStorage& InW);

	virtual void InitializeFromConstant(const FVoxelRuntimePinValue& Constant) override;
};