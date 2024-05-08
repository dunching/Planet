// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelBaseBuffers.h"
#include "VoxelFloatBuffers.generated.h"

DECLARE_VOXEL_BUFFER(FVoxelVector2DBuffer, FVector2D);

USTRUCT(DisplayName = "Vector2D Buffer")
struct VOXELGRAPHCORE_API FVoxelVector2DBuffer final : public FVoxelBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_BUFFER_BODY(FVoxelVector2DBuffer, FVector2D);

	UPROPERTY()
	FVoxelFloatBuffer X;

	UPROPERTY()
	FVoxelFloatBuffer Y;

	FORCEINLINE FVector2f operator[](const int32 Index) const
	{
		return FVector2f(X[Index], Y[Index]);
	}

	static FVoxelVector2DBuffer Make(const FVector2f& Value);
	static FVoxelVector2DBuffer Make(
		FVoxelFloatBufferStorage& InX,
		FVoxelFloatBufferStorage& InY);

	virtual void InitializeFromConstant(const FVoxelRuntimePinValue& Constant) override;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_BUFFER(FVoxelVectorBuffer, FVector);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelVectorBuffer final : public FVoxelBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_BUFFER_BODY(FVoxelVectorBuffer, FVector);

	UPROPERTY()
	FVoxelFloatBuffer X;

	UPROPERTY()
	FVoxelFloatBuffer Y;

	UPROPERTY()
	FVoxelFloatBuffer Z;

	FORCEINLINE FVector3f operator[](const int32 Index) const
	{
		return FVector3f(X[Index], Y[Index], Z[Index]);
	}

	static FVoxelVectorBuffer Make(const FVector3f& Value);
	static FVoxelVectorBuffer Make(
		FVoxelFloatBufferStorage& InX,
		FVoxelFloatBufferStorage& InY,
		FVoxelFloatBufferStorage& InZ);

	virtual void InitializeFromConstant(const FVoxelRuntimePinValue& Constant) override;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_BUFFER(FVoxelLinearColorBuffer, FLinearColor);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelLinearColorBuffer final : public FVoxelBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_BUFFER_BODY(FVoxelLinearColorBuffer, FLinearColor);

	UPROPERTY()
	FVoxelFloatBuffer R;

	UPROPERTY()
	FVoxelFloatBuffer G;

	UPROPERTY()
	FVoxelFloatBuffer B;

	UPROPERTY()
	FVoxelFloatBuffer A;

	FORCEINLINE FLinearColor operator[](const int32 Index) const
	{
		return FLinearColor(R[Index], G[Index], B[Index], A[Index]);
	}

	static FVoxelLinearColorBuffer Make(const FLinearColor& Value);
	static FVoxelLinearColorBuffer Make(
		FVoxelFloatBufferStorage& InR,
		FVoxelFloatBufferStorage& InG,
		FVoxelFloatBufferStorage& InB,
		FVoxelFloatBufferStorage& InA);

	virtual void InitializeFromConstant(const FVoxelRuntimePinValue& Constant) override;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_BUFFER(FVoxelQuaternionBuffer, FQuat);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelQuaternionBuffer final : public FVoxelBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_BUFFER_BODY(FVoxelQuaternionBuffer, FQuat);

	UPROPERTY()
	FVoxelFloatBuffer X;

	UPROPERTY()
	FVoxelFloatBuffer Y;

	UPROPERTY()
	FVoxelFloatBuffer Z;

	UPROPERTY()
	FVoxelFloatBuffer W;

	FORCEINLINE FQuat4f operator[](const int32 Index) const
	{
		return FQuat4f(X[Index], Y[Index], Z[Index], W[Index]);
	}

	static FVoxelQuaternionBuffer Make(const FQuat4f& Value);
	static FVoxelQuaternionBuffer Make(
		FVoxelFloatBufferStorage& InX,
		FVoxelFloatBufferStorage& InY,
		FVoxelFloatBufferStorage& InZ,
		FVoxelFloatBufferStorage& InW);

	virtual void InitializeFromConstant(const FVoxelRuntimePinValue& Constant) override;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_BUFFER(FVoxelTransformBuffer, FTransform);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTransformBuffer final : public FVoxelBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_BUFFER_BODY(FVoxelTransformBuffer, FTransform);

	UPROPERTY()
	FVoxelQuaternionBuffer Rotation;

	UPROPERTY()
	FVoxelVectorBuffer Translation;

	UPROPERTY()
	FVoxelVectorBuffer Scale;

	FORCEINLINE FTransform3f operator[](const int32 Index) const
	{
		return FTransform3f(Rotation[Index], Translation[Index], Scale[Index]);
	}
	virtual void InitializeFromConstant(const FVoxelRuntimePinValue& Constant) override;
};