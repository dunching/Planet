// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelBufferUtilities.h"
#include "VoxelFunctionLibrary.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "Buffer/VoxelDoubleBuffers.h"
#include "Buffer/VoxelIntegerBuffers.h"
#include "VoxelAutocastFunctionLibrary.generated.h"

UCLASS(Category = "Math|Conversions")
class VOXELGRAPHCORE_API UVoxelAutocastFunctionLibrary : public UVoxelFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(meta = (Autocast))
	FVoxelVectorBuffer Vector2DToVector(
		const FVoxelVector2DBuffer& Vector2D,
		const FVoxelFloatBuffer& Z) const
	{
		CheckVoxelBuffersNum_Function(Vector2D, Z);

		FVoxelVectorBuffer Result;
		Result.X = Vector2D.X;
		Result.Y = Vector2D.Y;
		Result.Z = Z;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelLinearColorBuffer Vector2DToLinearColor(
		const FVoxelVector2DBuffer& Vector2D,
		const FVoxelFloatBuffer& B,
		const FVoxelFloatBuffer& A) const
	{
		CheckVoxelBuffersNum_Function(Vector2D, B, A);

		FVoxelLinearColorBuffer Result;
		Result.R = Vector2D.X;
		Result.G = Vector2D.Y;
		Result.B = B;
		Result.A = A;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleVector2DBuffer Vector2DToDoubleVector2D(
		const FVoxelVector2DBuffer& Vector2D) const
	{
		FVoxelDoubleVector2DBuffer Result;
		Result.X = FVoxelBufferUtilities::FloatToDouble(Vector2D.X);
		Result.Y = FVoxelBufferUtilities::FloatToDouble(Vector2D.Y);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleVectorBuffer Vector2DToDoubleVector(
		const FVoxelVector2DBuffer& Vector2D,
		const FVoxelDoubleBuffer& Z) const
	{
		CheckVoxelBuffersNum_Function(Vector2D, Z);

		FVoxelDoubleVectorBuffer Result;
		Result.X = FVoxelBufferUtilities::FloatToDouble(Vector2D.X);
		Result.Y = FVoxelBufferUtilities::FloatToDouble(Vector2D.Y);
		Result.Z = Z;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleLinearColorBuffer Vector2DToDoubleLinearColor(
		const FVoxelVector2DBuffer& Vector2D,
		const FVoxelDoubleBuffer& B,
		const FVoxelDoubleBuffer& A) const
	{
		CheckVoxelBuffersNum_Function(Vector2D, B, A);

		FVoxelDoubleLinearColorBuffer Result;
		Result.R = FVoxelBufferUtilities::FloatToDouble(Vector2D.X);
		Result.G = FVoxelBufferUtilities::FloatToDouble(Vector2D.Y);
		Result.B = B;
		Result.A = A;
		return Result;
	}

public:
	UFUNCTION(meta = (Autocast))
	FVoxelVector2DBuffer VectorToVector2D(
		const FVoxelVectorBuffer& Vector) const
	{
		FVoxelVector2DBuffer Result;
		Result.X = Vector.X;
		Result.Y = Vector.Y;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelLinearColorBuffer VectorToLinearColor(
		const FVoxelVectorBuffer& Vector,
		const FVoxelFloatBuffer& A) const
	{
		CheckVoxelBuffersNum_Function(Vector, A);

		FVoxelLinearColorBuffer Result;
		Result.R = Vector.X;
		Result.G = Vector.Y;
		Result.B = Vector.Z;
		Result.A = A;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleVector2DBuffer VectorToDoubleVector2D(
		const FVoxelVectorBuffer& Vector) const
	{
		FVoxelDoubleVector2DBuffer Result;
		Result.X = FVoxelBufferUtilities::FloatToDouble(Vector.X);
		Result.Y = FVoxelBufferUtilities::FloatToDouble(Vector.Y);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleVectorBuffer VectorToDoubleVector(
		const FVoxelVectorBuffer& Vector) const
	{
		FVoxelDoubleVectorBuffer Result;
		Result.X = FVoxelBufferUtilities::FloatToDouble(Vector.X);
		Result.Y = FVoxelBufferUtilities::FloatToDouble(Vector.Y);
		Result.Z = FVoxelBufferUtilities::FloatToDouble(Vector.Z);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleLinearColorBuffer VectorToDoubleLinearColor(
		const FVoxelVectorBuffer& Vector,
		const FVoxelDoubleBuffer& A) const
	{
		CheckVoxelBuffersNum_Function(Vector, A);

		FVoxelDoubleLinearColorBuffer Result;
		Result.R = FVoxelBufferUtilities::FloatToDouble(Vector.X);
		Result.G = FVoxelBufferUtilities::FloatToDouble(Vector.Y);
		Result.B = FVoxelBufferUtilities::FloatToDouble(Vector.Z);
		Result.A = A;
		return Result;
	}

public:
	UFUNCTION(meta = (Autocast))
	FVoxelVector2DBuffer LinearColorToVector2D(
		const FVoxelLinearColorBuffer& Color) const
	{
		FVoxelVector2DBuffer Result;
		Result.X = Color.R;
		Result.Y = Color.G;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelVectorBuffer LinearColorToVector(
		const FVoxelLinearColorBuffer& Color) const
	{
		FVoxelVectorBuffer Result;
		Result.X = Color.R;
		Result.Y = Color.G;
		Result.Z = Color.B;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleVector2DBuffer LinearColorToDoubleVector2D(
		const FVoxelLinearColorBuffer& Color) const
	{
		FVoxelDoubleVector2DBuffer Result;
		Result.X = FVoxelBufferUtilities::FloatToDouble(Color.R);
		Result.Y = FVoxelBufferUtilities::FloatToDouble(Color.G);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleVectorBuffer LinearColorToDoubleVector(
		const FVoxelLinearColorBuffer& Color) const
	{
		FVoxelDoubleVectorBuffer Result;
		Result.X = FVoxelBufferUtilities::FloatToDouble(Color.R);
		Result.Y = FVoxelBufferUtilities::FloatToDouble(Color.G);
		Result.Z = FVoxelBufferUtilities::FloatToDouble(Color.B);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleLinearColorBuffer LinearColorToDoubleLinearColor(
		const FVoxelLinearColorBuffer& Color) const
	{
		FVoxelDoubleLinearColorBuffer Result;
		Result.R = FVoxelBufferUtilities::FloatToDouble(Color.R);
		Result.G = FVoxelBufferUtilities::FloatToDouble(Color.G);
		Result.B = FVoxelBufferUtilities::FloatToDouble(Color.B);
		Result.A = FVoxelBufferUtilities::FloatToDouble(Color.A);
		return Result;
	}

public:
	UFUNCTION(meta = (Autocast))
	FVoxelVector2DBuffer DoubleVector2DToVector2D(
		const FVoxelDoubleVector2DBuffer& Vector2D) const
	{
		FVoxelVector2DBuffer Result;
		Result.X = FVoxelBufferUtilities::DoubleToFloat(Vector2D.X);
		Result.Y = FVoxelBufferUtilities::DoubleToFloat(Vector2D.Y);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelVectorBuffer DoubleVector2DToVector(
		const FVoxelDoubleVector2DBuffer& Vector2D,
		const FVoxelFloatBuffer& Z) const
	{
		CheckVoxelBuffersNum_Function(Vector2D, Z);

		FVoxelVectorBuffer Result;
		Result.X = FVoxelBufferUtilities::DoubleToFloat(Vector2D.X);
		Result.Y = FVoxelBufferUtilities::DoubleToFloat(Vector2D.Y);
		Result.Z = Z;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelLinearColorBuffer DoubleVector2DToLinearColor(
		const FVoxelDoubleVector2DBuffer& Vector2D,
		const FVoxelFloatBuffer& B,
		const FVoxelFloatBuffer& A) const
	{
		CheckVoxelBuffersNum_Function(Vector2D, B, A);

		FVoxelLinearColorBuffer Result;
		Result.R = FVoxelBufferUtilities::DoubleToFloat(Vector2D.X);
		Result.G = FVoxelBufferUtilities::DoubleToFloat(Vector2D.Y);
		Result.B = B;
		Result.A = A;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleVectorBuffer DoubleVector2DToDoubleVector(
		const FVoxelDoubleVector2DBuffer& Vector2D,
		const FVoxelDoubleBuffer& Z) const
	{
		CheckVoxelBuffersNum_Function(Vector2D, Z);

		FVoxelDoubleVectorBuffer Result;
		Result.X = Vector2D.X;
		Result.Y = Vector2D.Y;
		Result.Z = Z;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleLinearColorBuffer DoubleVector2DToDoubleLinearColor(
		const FVoxelDoubleVector2DBuffer& Vector2D,
		const FVoxelDoubleBuffer& B,
		const FVoxelDoubleBuffer& A) const
	{
		CheckVoxelBuffersNum_Function(Vector2D, B, A);

		FVoxelDoubleLinearColorBuffer Result;
		Result.R = Vector2D.X;
		Result.G = Vector2D.Y;
		Result.B = B;
		Result.A = A;
		return Result;
	}

public:
	UFUNCTION(meta = (Autocast))
	FVoxelVector2DBuffer DoubleVectorToVector2D(
		const FVoxelDoubleVectorBuffer& Vector) const
	{
		FVoxelVector2DBuffer Result;
		Result.X = FVoxelBufferUtilities::DoubleToFloat(Vector.X);
		Result.Y = FVoxelBufferUtilities::DoubleToFloat(Vector.Y);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelVectorBuffer DoubleVectorToVector(
		const FVoxelDoubleVectorBuffer& Vector) const
	{
		FVoxelVectorBuffer Result;
		Result.X = FVoxelBufferUtilities::DoubleToFloat(Vector.X);
		Result.Y = FVoxelBufferUtilities::DoubleToFloat(Vector.Y);
		Result.Z = FVoxelBufferUtilities::DoubleToFloat(Vector.Z);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelLinearColorBuffer DoubleVectorToLinearColor(
		const FVoxelDoubleVectorBuffer& Vector,
		const FVoxelFloatBuffer& A) const
	{
		CheckVoxelBuffersNum_Function(Vector, A);

		FVoxelLinearColorBuffer Result;
		Result.R = FVoxelBufferUtilities::DoubleToFloat(Vector.X);
		Result.G = FVoxelBufferUtilities::DoubleToFloat(Vector.Y);
		Result.B = FVoxelBufferUtilities::DoubleToFloat(Vector.Z);
		Result.A = A;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleVector2DBuffer DoubleVectorToDoubleVector2D(
		const FVoxelDoubleVectorBuffer& Vector) const
	{
		FVoxelDoubleVector2DBuffer Result;
		Result.X = Vector.X;
		Result.Y = Vector.Y;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleLinearColorBuffer DoubleVectorToDoubleLinearColor(
		const FVoxelDoubleVectorBuffer& Vector,
		const FVoxelDoubleBuffer& A) const
	{
		CheckVoxelBuffersNum_Function(Vector, A);

		FVoxelDoubleLinearColorBuffer Result;
		Result.R = Vector.X;
		Result.G = Vector.Y;
		Result.B = Vector.Z;
		Result.A = A;
		return Result;
	}

public:
	UFUNCTION(meta = (Autocast))
	FVoxelVector2DBuffer DoubleLinearColorToVector2D(
		const FVoxelDoubleLinearColorBuffer& Color) const
	{
		FVoxelVector2DBuffer Result;
		Result.X = FVoxelBufferUtilities::DoubleToFloat(Color.R);
		Result.Y = FVoxelBufferUtilities::DoubleToFloat(Color.G);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelVectorBuffer DoubleLinearColorToVector(
		const FVoxelDoubleLinearColorBuffer& Color) const
	{
		FVoxelVectorBuffer Result;
		Result.X = FVoxelBufferUtilities::DoubleToFloat(Color.R);
		Result.Y = FVoxelBufferUtilities::DoubleToFloat(Color.G);
		Result.Z = FVoxelBufferUtilities::DoubleToFloat(Color.B);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelLinearColorBuffer DoubleLinearColorToLinearColor(
		const FVoxelDoubleLinearColorBuffer& Color) const
	{
		FVoxelLinearColorBuffer Result;
		Result.R = FVoxelBufferUtilities::DoubleToFloat(Color.R);
		Result.G = FVoxelBufferUtilities::DoubleToFloat(Color.G);
		Result.B = FVoxelBufferUtilities::DoubleToFloat(Color.B);
		Result.A = FVoxelBufferUtilities::DoubleToFloat(Color.A);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleVector2DBuffer DoubleLinearColorToDoubleVector2D(
		const FVoxelDoubleLinearColorBuffer& Color) const
	{
		FVoxelDoubleVector2DBuffer Result;
		Result.X = Color.R;
		Result.Y = Color.G;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleVectorBuffer DoubleLinearColorToDoubleVector(
		const FVoxelDoubleLinearColorBuffer& Color) const
	{
		FVoxelDoubleVectorBuffer Result;
		Result.X = Color.R;
		Result.Y = Color.G;
		Result.Z = Color.B;
		return Result;
	}

public:
	UFUNCTION(meta = (Autocast))
	FVoxelIntVectorBuffer IntPointToIntVector(
		const FVoxelIntPointBuffer& IntPoint,
		const FVoxelInt32Buffer& Z) const
	{
		CheckVoxelBuffersNum_Function(IntPoint, Z);

		FVoxelIntVectorBuffer Result;
		Result.X = IntPoint.X;
		Result.Y = IntPoint.Y;
		Result.Z = Z;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelIntVector4Buffer IntPointToIntVector4(
		const FVoxelIntPointBuffer& IntPoint,
		const FVoxelInt32Buffer& Z,
		const FVoxelInt32Buffer& W) const
	{
		CheckVoxelBuffersNum_Function(IntPoint, Z, W);

		FVoxelIntVector4Buffer Result;
		Result.X = IntPoint.X;
		Result.Y = IntPoint.Y;
		Result.Z = Z;
		Result.W = W;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelVector2DBuffer IntPointToVector2D(
		const FVoxelIntPointBuffer& IntPoint) const
	{
		FVoxelVector2DBuffer Result;
		Result.X = FVoxelBufferUtilities::IntToFloat(IntPoint.X);
		Result.Y = FVoxelBufferUtilities::IntToFloat(IntPoint.Y);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelVectorBuffer IntPointToVector(
		const FVoxelIntPointBuffer& IntPoint,
		const FVoxelFloatBuffer& Z) const
	{
		CheckVoxelBuffersNum_Function(IntPoint, Z);

		FVoxelVectorBuffer Result;
		Result.X = FVoxelBufferUtilities::IntToFloat(IntPoint.X);
		Result.Y = FVoxelBufferUtilities::IntToFloat(IntPoint.Y);
		Result.Z = Z;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelLinearColorBuffer IntPointToLinearColor(
		const FVoxelIntPointBuffer& IntPoint,
		const FVoxelFloatBuffer& B,
		const FVoxelFloatBuffer& A) const
	{
		CheckVoxelBuffersNum_Function(IntPoint, B, A);

		FVoxelLinearColorBuffer Result;
		Result.R = FVoxelBufferUtilities::IntToFloat(IntPoint.X);
		Result.G = FVoxelBufferUtilities::IntToFloat(IntPoint.Y);
		Result.B = B;
		Result.A = A;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleVector2DBuffer IntPointToDoubleVector2D(
		const FVoxelIntPointBuffer& IntPoint) const
	{
		FVoxelDoubleVector2DBuffer Result;
		Result.X = FVoxelBufferUtilities::IntToDouble(IntPoint.X);
		Result.Y = FVoxelBufferUtilities::IntToDouble(IntPoint.Y);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleVectorBuffer IntPointToDoubleVector(
		const FVoxelIntPointBuffer& IntPoint,
		const FVoxelDoubleBuffer& Z) const
	{
		CheckVoxelBuffersNum_Function(IntPoint, Z);

		FVoxelDoubleVectorBuffer Result;
		Result.X = FVoxelBufferUtilities::IntToDouble(IntPoint.X);
		Result.Y = FVoxelBufferUtilities::IntToDouble(IntPoint.Y);
		Result.Z = Z;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleLinearColorBuffer IntPointToDoubleLinearColor(
		const FVoxelIntPointBuffer& IntPoint,
		const FVoxelDoubleBuffer& B,
		const FVoxelDoubleBuffer& A) const
	{
		CheckVoxelBuffersNum_Function(IntPoint, B, A);

		FVoxelDoubleLinearColorBuffer Result;
		Result.R = FVoxelBufferUtilities::IntToDouble(IntPoint.X);
		Result.G = FVoxelBufferUtilities::IntToDouble(IntPoint.Y);
		Result.B = B;
		Result.A = A;
		return Result;
	}

public:
	UFUNCTION(meta = (Autocast))
	FVoxelIntPointBuffer IntVectorToIntPoint(
		const FVoxelIntVectorBuffer& Vector) const
	{
		FVoxelIntPointBuffer Result;
		Result.X = Vector.X;
		Result.Y = Vector.Y;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelIntVector4Buffer IntVectorToIntVector4(
		const FVoxelIntVectorBuffer& Vector,
		const FVoxelInt32Buffer& W) const
	{
		CheckVoxelBuffersNum_Function(Vector, W);

		FVoxelIntVector4Buffer Result;
		Result.X = Vector.X;
		Result.Y = Vector.Y;
		Result.Z = Vector.Z;
		Result.W = W;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelVector2DBuffer IntVectorToVector2D(
		const FVoxelIntVectorBuffer& Vector) const
	{
		FVoxelVector2DBuffer Result;
		Result.X = FVoxelBufferUtilities::IntToFloat(Vector.X);
		Result.Y = FVoxelBufferUtilities::IntToFloat(Vector.Y);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelVectorBuffer IntVectorToVector(
		const FVoxelIntVectorBuffer& Vector) const
	{
		FVoxelVectorBuffer Result;
		Result.X = FVoxelBufferUtilities::IntToFloat(Vector.X);
		Result.Y = FVoxelBufferUtilities::IntToFloat(Vector.Y);
		Result.Z = FVoxelBufferUtilities::IntToFloat(Vector.Z);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelLinearColorBuffer IntVectorToLinearColor(
		const FVoxelIntVectorBuffer& Vector,
		const FVoxelFloatBuffer& A) const
	{
		CheckVoxelBuffersNum_Function(Vector, A);

		FVoxelLinearColorBuffer Result;
		Result.R = FVoxelBufferUtilities::IntToFloat(Vector.X);
		Result.G = FVoxelBufferUtilities::IntToFloat(Vector.Y);
		Result.B = FVoxelBufferUtilities::IntToFloat(Vector.Z);
		Result.A = A;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleVector2DBuffer IntVectorToDoubleVector2D(
		const FVoxelIntVectorBuffer& Vector) const
	{
		FVoxelDoubleVector2DBuffer Result;
		Result.X = FVoxelBufferUtilities::IntToDouble(Vector.X);
		Result.Y = FVoxelBufferUtilities::IntToDouble(Vector.Y);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleVectorBuffer IntVectorToDoubleVector(
		const FVoxelIntVectorBuffer& Vector) const
	{
		FVoxelDoubleVectorBuffer Result;
		Result.X = FVoxelBufferUtilities::IntToDouble(Vector.X);
		Result.Y = FVoxelBufferUtilities::IntToDouble(Vector.Y);
		Result.Z = FVoxelBufferUtilities::IntToDouble(Vector.Z);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleLinearColorBuffer IntVectorToDoubleLinearColor(
		const FVoxelIntVectorBuffer& Vector,
		const FVoxelDoubleBuffer& A) const
	{
		CheckVoxelBuffersNum_Function(Vector, A);

		FVoxelDoubleLinearColorBuffer Result;
		Result.R = FVoxelBufferUtilities::IntToDouble(Vector.X);
		Result.G = FVoxelBufferUtilities::IntToDouble(Vector.Y);
		Result.B = FVoxelBufferUtilities::IntToDouble(Vector.Z);
		Result.A = A;
		return Result;
	}

public:
	UFUNCTION(meta = (Autocast))
	FVoxelIntPointBuffer IntVector4ToIntPoint(
		const FVoxelIntVector4Buffer& Vector) const
	{
		FVoxelIntPointBuffer Result;
		Result.X = Vector.X;
		Result.Y = Vector.Y;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelIntVectorBuffer IntVector4ToIntVector(
		const FVoxelIntVector4Buffer& Vector) const
	{
		FVoxelIntVectorBuffer Result;
		Result.X = Vector.X;
		Result.Y = Vector.Y;
		Result.Z = Vector.Z;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelVector2DBuffer IntVector4ToVector2D(
		const FVoxelIntVector4Buffer& Vector) const
	{
		FVoxelVector2DBuffer Result;
		Result.X = FVoxelBufferUtilities::IntToFloat(Vector.X);
		Result.Y = FVoxelBufferUtilities::IntToFloat(Vector.Y);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelVectorBuffer IntVector4ToVector(
		const FVoxelIntVector4Buffer& Vector) const
	{
		FVoxelVectorBuffer Result;
		Result.X = FVoxelBufferUtilities::IntToFloat(Vector.X);
		Result.Y = FVoxelBufferUtilities::IntToFloat(Vector.Y);
		Result.Z = FVoxelBufferUtilities::IntToFloat(Vector.Z);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelLinearColorBuffer IntVector4ToLinearColor(
		const FVoxelIntVector4Buffer& Vector) const
	{
		FVoxelLinearColorBuffer Result;
		Result.R = FVoxelBufferUtilities::IntToFloat(Vector.X);
		Result.G = FVoxelBufferUtilities::IntToFloat(Vector.Y);
		Result.B = FVoxelBufferUtilities::IntToFloat(Vector.Z);
		Result.A = FVoxelBufferUtilities::IntToFloat(Vector.W);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleVector2DBuffer IntVector4ToDoubleVector2D(
		const FVoxelIntVector4Buffer& Vector) const
	{
		FVoxelDoubleVector2DBuffer Result;
		Result.X = FVoxelBufferUtilities::IntToDouble(Vector.X);
		Result.Y = FVoxelBufferUtilities::IntToDouble(Vector.Y);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleVectorBuffer IntVector4ToDoubleVector(
		const FVoxelIntVector4Buffer& Vector) const
	{
		FVoxelDoubleVectorBuffer Result;
		Result.X = FVoxelBufferUtilities::IntToDouble(Vector.X);
		Result.Y = FVoxelBufferUtilities::IntToDouble(Vector.Y);
		Result.Z = FVoxelBufferUtilities::IntToDouble(Vector.Z);
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleLinearColorBuffer IntVector4ToDoubleLinearColor(
		const FVoxelIntVector4Buffer& Vector) const
	{
		FVoxelDoubleLinearColorBuffer Result;
		Result.R = FVoxelBufferUtilities::IntToDouble(Vector.X);
		Result.G = FVoxelBufferUtilities::IntToDouble(Vector.Y);
		Result.B = FVoxelBufferUtilities::IntToDouble(Vector.Z);
		Result.A = FVoxelBufferUtilities::IntToDouble(Vector.W);
		return Result;
	}

public:
	UFUNCTION(meta = (Autocast))
	FVoxelVectorBuffer FloatToVector(const FVoxelFloatBuffer& Value) const
	{
		FVoxelVectorBuffer Result;
		Result.X = Value;
		Result.Y = Value;
		Result.Z = Value;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelVector2DBuffer FloatToVector2D(const FVoxelFloatBuffer& Value) const
	{
		FVoxelVector2DBuffer Result;
		Result.X = Value;
		Result.Y = Value;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelLinearColorBuffer FloatToLinearColor(const FVoxelFloatBuffer& Value) const
	{
		FVoxelLinearColorBuffer Result;
		Result.R = Value;
		Result.G = Value;
		Result.B = Value;
		Result.A = Value;
		return Result;
	}

public:
	UFUNCTION(meta = (Autocast))
	FVoxelDoubleVectorBuffer DoubleToDoubleVector(const FVoxelDoubleBuffer& Value) const
	{
		FVoxelDoubleVectorBuffer Result;
		Result.X = Value;
		Result.Y = Value;
		Result.Z = Value;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleVector2DBuffer DoubleToDoubleVector2D(const FVoxelDoubleBuffer& Value) const
	{
		FVoxelDoubleVector2DBuffer Result;
		Result.X = Value;
		Result.Y = Value;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleLinearColorBuffer DoubleToDoubleLinearColor(const FVoxelDoubleBuffer& Value) const
	{
		FVoxelDoubleLinearColorBuffer Result;
		Result.R = Value;
		Result.G = Value;
		Result.B = Value;
		Result.A = Value;
		return Result;
	}

public:
	UFUNCTION(meta = (Autocast))
	FVoxelIntPointBuffer IntegerToIntPoint(const FVoxelInt32Buffer& Value) const
	{
		FVoxelIntPointBuffer Result;
		Result.X = Value;
		Result.Y = Value;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelIntVectorBuffer IntegerToIntVector(const FVoxelInt32Buffer& Value) const
	{
		FVoxelIntVectorBuffer Result;
		Result.X = Value;
		Result.Y = Value;
		Result.Z = Value;
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelIntVector4Buffer IntegerToIntVector4(const FVoxelInt32Buffer& Value) const
	{
		FVoxelIntVector4Buffer Result;
		Result.X = Value;
		Result.Y = Value;
		Result.Z = Value;
		Result.W = Value;
		return Result;
	}

public:
	UFUNCTION(meta = (Autocast))
	FVoxelFloatBuffer IntegerToFloat(const FVoxelInt32Buffer& Value) const
	{
		return FVoxelBufferUtilities::IntToFloat(Value);
	}
	UFUNCTION(meta = (Autocast))
	FVoxelDoubleBuffer IntegerToDouble(const FVoxelInt32Buffer& Value) const
	{
		return FVoxelBufferUtilities::IntToDouble(Value);
	}

	UFUNCTION(meta = (Autocast))
	FVoxelDoubleBuffer FloatToDouble(const FVoxelFloatBuffer& Value) const
	{
		return FVoxelBufferUtilities::FloatToDouble(Value);
	}
	UFUNCTION(meta = (Autocast))
	FVoxelFloatBuffer DoubleToFloat(const FVoxelDoubleBuffer& Value) const
	{
		return FVoxelBufferUtilities::DoubleToFloat(Value);
	}

public:
	UFUNCTION(meta = (Autocast))
	FVoxelSeedBuffer IntegerToSeed(const FVoxelInt32Buffer& Value) const
	{
		FVoxelSeedBuffer Result;
		Result.SetStorage(Value.GetSharedStorage());
		return Result;
	}

	UFUNCTION(meta = (Autocast))
	FVoxelInt32Buffer SeedToInteger(const FVoxelSeedBuffer& Value) const
	{
		FVoxelInt32Buffer Result;
		Result.SetStorage(Value.GetSharedStorage());
		return Result;
	}
};