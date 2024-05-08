// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelBaseBuffers.h"
#include "VoxelDoubleBuffers.generated.h"

USTRUCT(BlueprintType)
struct VOXELGRAPHCORE_API FVoxelDoubleVector2D
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vector2D")
	double X = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vector2D")
	double Y = 0;

	FVoxelDoubleVector2D() = default;
	FORCEINLINE FVoxelDoubleVector2D(const FVector2d& Vector)
		: X(Vector.X)
		, Y(Vector.Y)
	{
	}
	FORCEINLINE FVoxelDoubleVector2D(
		const double X,
		const double Y)
		: X(X)
		, Y(Y)
	{
	}
};

DECLARE_VOXEL_BUFFER(FVoxelDoubleVector2DBuffer, FVoxelDoubleVector2D);

USTRUCT(DisplayName = "Double Vector2D Buffer")
struct VOXELGRAPHCORE_API FVoxelDoubleVector2DBuffer final : public FVoxelBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_BUFFER_BODY(FVoxelDoubleVector2DBuffer, FVoxelDoubleVector2D);

	UPROPERTY()
	FVoxelDoubleBuffer X;

	UPROPERTY()
	FVoxelDoubleBuffer Y;

	FORCEINLINE FVoxelDoubleVector2D operator[](const int32 Index) const
	{
		return FVoxelDoubleVector2D(X[Index], Y[Index]);
	}

	static FVoxelDoubleVector2DBuffer Make(const FVoxelDoubleVector2D& Value);
	static FVoxelDoubleVector2DBuffer Make(
		FVoxelDoubleBufferStorage& InX,
		FVoxelDoubleBufferStorage& InY);

	virtual void InitializeFromConstant(const FVoxelRuntimePinValue& Constant) override;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct VOXELGRAPHCORE_API FVoxelDoubleVector
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vector")
	double X = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vector")
	double Y = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vector")
	double Z = 0;

	FVoxelDoubleVector() = default;
	FORCEINLINE FVoxelDoubleVector(const FVector& Vector)
		: X(Vector.X)
		, Y(Vector.Y)
		, Z(Vector.Z)
	{
	}
	FORCEINLINE FVoxelDoubleVector(
		const double X,
		const double Y,
		const double Z)
		: X(X)
		, Y(Y)
		, Z(Z)
	{
	}
};

DECLARE_VOXEL_BUFFER(FVoxelDoubleVectorBuffer, FVoxelDoubleVector);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelDoubleVectorBuffer final : public FVoxelBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_BUFFER_BODY(FVoxelDoubleVectorBuffer, FVoxelDoubleVector);

	UPROPERTY()
	FVoxelDoubleBuffer X;

	UPROPERTY()
	FVoxelDoubleBuffer Y;

	UPROPERTY()
	FVoxelDoubleBuffer Z;

	FORCEINLINE FVoxelDoubleVector operator[](const int32 Index) const
	{
		return FVoxelDoubleVector(X[Index], Y[Index], Z[Index]);
	}

	static FVoxelDoubleVectorBuffer Make(const FVoxelDoubleVector& Value);
	static FVoxelDoubleVectorBuffer Make(
		FVoxelDoubleBufferStorage& InX,
		FVoxelDoubleBufferStorage& InY,
		FVoxelDoubleBufferStorage& InZ);

	virtual void InitializeFromConstant(const FVoxelRuntimePinValue& Constant) override;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct VOXELGRAPHCORE_API FVoxelDoubleQuat
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quat")
	double X = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quat")
	double Y = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quat")
	double Z = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quat")
	double W = 0;

	FVoxelDoubleQuat() = default;
	FORCEINLINE FVoxelDoubleQuat(const FQuat4d& Quat)
		: X(Quat.X)
		, Y(Quat.Y)
		, Z(Quat.Z)
		, W(Quat.W)
	{
	}
	FORCEINLINE FVoxelDoubleQuat(
		const double X,
		const double Y,
		const double Z,
		const double W)
		: X(X)
		, Y(Y)
		, Z(Z)
		, W(W)
	{
	}
};

DECLARE_VOXEL_BUFFER(FVoxelDoubleQuaternionBuffer, FVoxelDoubleQuat);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelDoubleQuaternionBuffer final : public FVoxelBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_BUFFER_BODY(FVoxelDoubleQuaternionBuffer, FVoxelDoubleQuat);

	UPROPERTY()
	FVoxelDoubleBuffer X;

	UPROPERTY()
	FVoxelDoubleBuffer Y;

	UPROPERTY()
	FVoxelDoubleBuffer Z;

	UPROPERTY()
	FVoxelDoubleBuffer W;

	FORCEINLINE FVoxelDoubleQuat operator[](const int32 Index) const
	{
		return FVoxelDoubleQuat(X[Index], Y[Index], Z[Index], W[Index]);
	}

	static FVoxelDoubleQuaternionBuffer Make(const FVoxelDoubleQuat& Value);
	static FVoxelDoubleQuaternionBuffer Make(
		FVoxelDoubleBufferStorage& InX,
		FVoxelDoubleBufferStorage& InY,
		FVoxelDoubleBufferStorage& InZ,
		FVoxelDoubleBufferStorage& InW);

	virtual void InitializeFromConstant(const FVoxelRuntimePinValue& Constant) override;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct VOXELGRAPHCORE_API FVoxelDoubleTransform
{
	GENERATED_BODY()

	// Rotation of this transformation, as a quaternion
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Transform)
	FVoxelDoubleQuat Rotation = FQuat::Identity;

	// Translation of this transformation, as a vector
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Transform)
	FVoxelDoubleVector Translation = FVector::ZeroVector;

	// 3D scale (always applied in local space) as a vector
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Transform)
	FVoxelDoubleVector Scale3D = FVector::ZeroVector;

	FVoxelDoubleTransform() = default;
	FORCEINLINE FVoxelDoubleTransform(
		const FVoxelDoubleQuat& Rotation,
		const FVoxelDoubleVector& Translation,
		const FVoxelDoubleVector& Scale3D)
		: Rotation(Rotation)
		, Translation(Translation)
		, Scale3D(Scale3D)
	{
	}
};

DECLARE_VOXEL_BUFFER(FVoxelDoubleTransformBuffer, FVoxelDoubleTransform);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelDoubleTransformBuffer final : public FVoxelBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_BUFFER_BODY(FVoxelDoubleTransformBuffer, FVoxelDoubleTransform);

	UPROPERTY()
	FVoxelDoubleQuaternionBuffer Rotation;

	UPROPERTY()
	FVoxelDoubleVectorBuffer Translation;

	UPROPERTY()
	FVoxelDoubleVectorBuffer Scale;

	FORCEINLINE FVoxelDoubleTransform operator[](const int32 Index) const
	{
		return FVoxelDoubleTransform(Rotation[Index], Translation[Index], Scale[Index]);
	}
	virtual void InitializeFromConstant(const FVoxelRuntimePinValue& Constant) override;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct VOXELGRAPHCORE_API FVoxelDoubleLinearColor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LinearColor)
	double R = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LinearColor)
	double G = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LinearColor)
	double B = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LinearColor)
	double A = 0;

	FVoxelDoubleLinearColor() = default;
	FORCEINLINE FVoxelDoubleLinearColor(
		const double R,
		const double G,
		const double B,
		const double A)
		: R(R)
		, G(G)
		, B(B)
		, A(A)
	{
	}
};

DECLARE_VOXEL_BUFFER(FVoxelDoubleLinearColorBuffer, FVoxelDoubleLinearColor);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelDoubleLinearColorBuffer final : public FVoxelBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_BUFFER_BODY(FVoxelDoubleLinearColorBuffer, FVoxelDoubleLinearColor);

	UPROPERTY()
	FVoxelDoubleBuffer R;

	UPROPERTY()
	FVoxelDoubleBuffer G;

	UPROPERTY()
	FVoxelDoubleBuffer B;

	UPROPERTY()
	FVoxelDoubleBuffer A;

	FORCEINLINE FVoxelDoubleLinearColor operator[](const int32 Index) const
	{
		return FVoxelDoubleLinearColor(R[Index], G[Index], B[Index], A[Index]);
	}

	static FVoxelDoubleLinearColorBuffer Make(const FLinearColor& Value);
	static FVoxelDoubleLinearColorBuffer Make(
		FVoxelDoubleBufferStorage& InR,
		FVoxelDoubleBufferStorage& InG,
		FVoxelDoubleBufferStorage& InB,
		FVoxelDoubleBufferStorage& InA);

	virtual void InitializeFromConstant(const FVoxelRuntimePinValue& Constant) override;
};