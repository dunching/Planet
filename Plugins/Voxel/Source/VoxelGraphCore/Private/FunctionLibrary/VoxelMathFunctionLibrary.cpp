// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "FunctionLibrary/VoxelMathFunctionLibrary.h"
#include "VoxelGraphMigration.h"
#include "VoxelBufferUtilities.h"
#include "VoxelMathFunctionLibraryImpl.ispc.generated.h"

FVoxelTransformBuffer UVoxelMathFunctionLibrary::MakeTransform(
	const FVoxelVectorBuffer& Translation,
	const FVoxelQuaternionBuffer& Rotation,
	const FVoxelVectorBuffer& Scale) const
{
	CheckVoxelBuffersNum_Function(Translation, Rotation, Scale);

	FVoxelTransformBuffer Result;
	Result.Translation = Translation;
	Result.Rotation = Rotation;
	Result.Scale = Scale;
	return Result;
}

void UVoxelMathFunctionLibrary::BreakTransform(
	const FVoxelTransformBuffer& Value,
	FVoxelVectorBuffer& Translation,
	FVoxelQuaternionBuffer& Rotation,
	FVoxelVectorBuffer& Scale) const
{
	Translation = Value.Translation;
	Rotation = Value.Rotation;
	Scale = Value.Scale;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelVector2DBuffer UVoxelMathFunctionLibrary::MakeVector2D(
	const FVoxelFloatBuffer& X,
	const FVoxelFloatBuffer& Y) const
{
	CheckVoxelBuffersNum_Function(X, Y);

	FVoxelVector2DBuffer Result;
	Result.X = X;
	Result.Y = Y;
	return Result;
}

void UVoxelMathFunctionLibrary::BreakVector2D(
	const FVoxelVector2DBuffer& Value,
	FVoxelFloatBuffer& X,
	FVoxelFloatBuffer& Y) const
{
	X = Value.X;
	Y = Value.Y;
}

FVoxelVectorBuffer UVoxelMathFunctionLibrary::MakeVector(
	const FVoxelFloatBuffer& X,
	const FVoxelFloatBuffer& Y,
	const FVoxelFloatBuffer& Z) const
{
	CheckVoxelBuffersNum_Function(X, Y, Z);

	FVoxelVectorBuffer Result;
	Result.X = X;
	Result.Y = Y;
	Result.Z = Z;
	return Result;
}

void UVoxelMathFunctionLibrary::BreakVector(
	const FVoxelVectorBuffer& Value,
	FVoxelFloatBuffer& X,
	FVoxelFloatBuffer& Y,
	FVoxelFloatBuffer& Z) const
{
	X = Value.X;
	Y = Value.Y;
	Z = Value.Z;
}

FVoxelLinearColorBuffer UVoxelMathFunctionLibrary::MakeLinearColor(
	const FVoxelFloatBuffer& R,
	const FVoxelFloatBuffer& G,
	const FVoxelFloatBuffer& B,
	const FVoxelFloatBuffer& A) const
{
	CheckVoxelBuffersNum_Function(R, G, B, A);

	FVoxelLinearColorBuffer Result;
	Result.R = R;
	Result.G = G;
	Result.B = B;
	Result.A = A;
	return Result;
}

void UVoxelMathFunctionLibrary::BreakLinearColor(
	const FVoxelLinearColorBuffer& Value,
	FVoxelFloatBuffer& R,
	FVoxelFloatBuffer& G,
	FVoxelFloatBuffer& B,
	FVoxelFloatBuffer& A) const
{
	R = Value.R;
	G = Value.G;
	B = Value.B;
	A = Value.A;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelIntPointBuffer UVoxelMathFunctionLibrary::MakeIntPoint(
	const FVoxelInt32Buffer& X,
	const FVoxelInt32Buffer& Y) const
{
	CheckVoxelBuffersNum_Function(X, Y);

	FVoxelIntPointBuffer Result;
	Result.X = X;
	Result.Y = Y;
	return Result;
}

void UVoxelMathFunctionLibrary::BreakIntPoint(
	const FVoxelIntPointBuffer& Value,
	FVoxelInt32Buffer& X,
	FVoxelInt32Buffer& Y) const
{
	X = Value.X;
	Y = Value.Y;
}

FVoxelIntVectorBuffer UVoxelMathFunctionLibrary::MakeIntVector(
	const FVoxelInt32Buffer& X,
	const FVoxelInt32Buffer& Y,
	const FVoxelInt32Buffer& Z) const
{
	CheckVoxelBuffersNum_Function(X, Y, Z);

	FVoxelIntVectorBuffer Result;
	Result.X = X;
	Result.Y = Y;
	Result.Z = Z;
	return Result;
}

void UVoxelMathFunctionLibrary::BreakIntVector(
	const FVoxelIntVectorBuffer& Value,
	FVoxelInt32Buffer& X,
	FVoxelInt32Buffer& Y,
	FVoxelInt32Buffer& Z) const
{
	X = Value.X;
	Y = Value.Y;
	Z = Value.Z;
}

FVoxelIntVector4Buffer UVoxelMathFunctionLibrary::MakeIntVector4(
	const FVoxelInt32Buffer& X,
	const FVoxelInt32Buffer& Y,
	const FVoxelInt32Buffer& Z,
	const FVoxelInt32Buffer& W) const
{
	CheckVoxelBuffersNum_Function(X, Y, Z, W);

	FVoxelIntVector4Buffer Result;
	Result.X = X;
	Result.Y = Y;
	Result.Z = Z;
	Result.W = W;
	return Result;
}

void UVoxelMathFunctionLibrary::BreakIntVector4(
	const FVoxelIntVector4Buffer& Value,
	FVoxelInt32Buffer& X,
	FVoxelInt32Buffer& Y,
	FVoxelInt32Buffer& Z,
	FVoxelInt32Buffer& W) const
{
	X = Value.X;
	Y = Value.Y;
	Z = Value.Z;
	W = Value.W;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelDoubleVector2DBuffer UVoxelMathFunctionLibrary::MakeDoubleVector2D(
	const FVoxelDoubleBuffer& X,
	const FVoxelDoubleBuffer& Y) const
{
	CheckVoxelBuffersNum_Function(X, Y);

	FVoxelDoubleVector2DBuffer Result;
	Result.X = X;
	Result.Y = Y;
	return Result;
}

void UVoxelMathFunctionLibrary::BreakDoubleVector2D(
	const FVoxelDoubleVector2DBuffer& Value,
	FVoxelDoubleBuffer& X,
	FVoxelDoubleBuffer& Y) const
{
	X = Value.X;
	Y = Value.Y;
}

FVoxelDoubleVectorBuffer UVoxelMathFunctionLibrary::MakeDoubleVector(
	const FVoxelDoubleBuffer& X,
	const FVoxelDoubleBuffer& Y,
	const FVoxelDoubleBuffer& Z) const
{
	CheckVoxelBuffersNum_Function(X, Y, Z);

	FVoxelDoubleVectorBuffer Result;
	Result.X = X;
	Result.Y = Y;
	Result.Z = Z;
	return Result;
}

void UVoxelMathFunctionLibrary::BreakDoubleVector(
	const FVoxelDoubleVectorBuffer& Value,
	FVoxelDoubleBuffer& X,
	FVoxelDoubleBuffer& Y,
	FVoxelDoubleBuffer& Z) const
{
	X = Value.X;
	Y = Value.Y;
	Z = Value.Z;
}

FVoxelDoubleLinearColorBuffer UVoxelMathFunctionLibrary::MakeDoubleLinearColor(
	const FVoxelDoubleBuffer& R,
	const FVoxelDoubleBuffer& G,
	const FVoxelDoubleBuffer& B,
	const FVoxelDoubleBuffer& A) const
{
	CheckVoxelBuffersNum_Function(R, G, B, A);

	FVoxelDoubleLinearColorBuffer Result;
	Result.R = R;
	Result.G = G;
	Result.B = B;
	Result.A = A;
	return Result;
}

void UVoxelMathFunctionLibrary::BreakDoubleLinearColor(
	const FVoxelDoubleLinearColorBuffer& Value,
	FVoxelDoubleBuffer& R,
	FVoxelDoubleBuffer& G,
	FVoxelDoubleBuffer& B,
	FVoxelDoubleBuffer& A) const
{
	R = Value.R;
	G = Value.G;
	B = Value.B;
	A = Value.A;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelFloatRangeBuffer UVoxelMathFunctionLibrary::MakeFloatRange(
	const FVoxelFloatBuffer& Min,
	const FVoxelFloatBuffer& Max) const
{
	CheckVoxelBuffersNum_Function(Min, Max);

	FVoxelFloatRangeBuffer Result;
	Result.Min = Min;
	Result.Max = Max;
	return Result;
}

void UVoxelMathFunctionLibrary::BreakFloatRange(
	const FVoxelFloatRangeBuffer& Value,
	FVoxelFloatBuffer& Min,
	FVoxelFloatBuffer& Max) const
{
	Min = Value.Min;
	Max = Value.Max;
}

FVoxelInt32RangeBuffer UVoxelMathFunctionLibrary::MakeInt32Range(
	const FVoxelInt32Buffer& Min,
	const FVoxelInt32Buffer& Max) const
{
	CheckVoxelBuffersNum_Function(Min, Max);

	FVoxelInt32RangeBuffer Result;
	Result.Min = Min;
	Result.Max = Max;
	return Result;
}

void UVoxelMathFunctionLibrary::BreakInt32Range(
	const FVoxelInt32RangeBuffer& Value,
	FVoxelInt32Buffer& Min,
	FVoxelInt32Buffer& Max) const
{
	Min = Value.Min;
	Max = Value.Max;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelMathFunctionLibrary::SmoothMin(
	FVoxelFloatBuffer& OutDistance,
	FVoxelFloatBuffer& OutAlpha,
	const FVoxelFloatBuffer& A,
	const FVoxelFloatBuffer& B,
	const FVoxelFloatBuffer& Smoothness) const
{
	const int32 Num = ComputeVoxelBuffersNum_Function_void(A, B, Smoothness);

	FVoxelFloatBufferStorage Distance;
	Distance.Allocate(Num);

	FVoxelFloatBufferStorage Alpha;
	Alpha.Allocate(Num);

	ForeachVoxelBufferChunk(Num, [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelMathFunctionLibrary_SmoothMin(
			A.GetData(Iterator),
			A.IsConstant(),
			B.GetData(Iterator),
			B.IsConstant(),
			Smoothness.GetData(Iterator),
			Smoothness.IsConstant(),
			Iterator.Num(),
			Alpha.GetData(Iterator),
			Distance.GetData(Iterator));
	});

	OutDistance = FVoxelFloatBuffer::Make(Distance);
	OutAlpha = FVoxelFloatBuffer::Make(Alpha);
}

void UVoxelMathFunctionLibrary::SmoothMax(
	FVoxelFloatBuffer& OutDistance,
	FVoxelFloatBuffer& OutAlpha,
	const FVoxelFloatBuffer& A,
	const FVoxelFloatBuffer& B,
	const FVoxelFloatBuffer& Smoothness) const
{
	const int32 Num = ComputeVoxelBuffersNum_Function_void(A, B, Smoothness);

	FVoxelFloatBufferStorage Distance;
	Distance.Allocate(Num);

	FVoxelFloatBufferStorage Alpha;
	Alpha.Allocate(Num);

	ForeachVoxelBufferChunk(Num, [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelMathFunctionLibrary_SmoothMax(
			A.GetData(Iterator),
			A.IsConstant(),
			B.GetData(Iterator),
			B.IsConstant(),
			Smoothness.GetData(Iterator),
			Smoothness.IsConstant(),
			Iterator.Num(),
			Alpha.GetData(Iterator),
			Distance.GetData(Iterator));
	});

	OutDistance = FVoxelFloatBuffer::Make(Distance);
	OutAlpha = FVoxelFloatBuffer::Make(Alpha);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelQuaternionBuffer UVoxelMathFunctionLibrary::CombineRotation(
	const FVoxelQuaternionBuffer& A,
	const FVoxelQuaternionBuffer& B) const
{
	CheckVoxelBuffersNum_Function(A, B);
	return FVoxelBufferUtilities::Combine(A, B);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelVectorBuffer UVoxelMathFunctionLibrary::TransformLocation(
	const FVoxelVectorBuffer& Location,
	const FTransform& Transform) const
{
	return FVoxelBufferUtilities::ApplyTransform(Location, Transform);
}

FVoxelVectorBuffer UVoxelMathFunctionLibrary::InverseTransformLocation(
	const FVoxelVectorBuffer& Location,
	const FTransform& Transform) const
{
	return FVoxelBufferUtilities::ApplyInverseTransform(Location, Transform);
}