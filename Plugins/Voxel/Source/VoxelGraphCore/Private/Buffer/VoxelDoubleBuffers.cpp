// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Buffer/VoxelDoubleBuffers.h"

FVoxelDoubleVector2DBuffer FVoxelDoubleVector2DBuffer::Make(const FVoxelDoubleVector2D& Value)
{
	FVoxelDoubleVector2DBuffer Result;
	Result.X = FVoxelDoubleBuffer::Make(Value.X);
	Result.Y = FVoxelDoubleBuffer::Make(Value.Y);
	return Result;
}

FVoxelDoubleVector2DBuffer FVoxelDoubleVector2DBuffer::Make(
	FVoxelDoubleBufferStorage& InX,
	FVoxelDoubleBufferStorage& InY)
{
	FVoxelDoubleVector2DBuffer Result;
	Result.X = FVoxelDoubleBuffer::Make(InX);
	Result.Y = FVoxelDoubleBuffer::Make(InY);
	return Result;
}

void FVoxelDoubleVector2DBuffer::InitializeFromConstant(const FVoxelRuntimePinValue& Constant)
{
	X = FVoxelDoubleBuffer::Make(Constant.Get<UniformType>().X);
	Y = FVoxelDoubleBuffer::Make(Constant.Get<UniformType>().Y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelDoubleVectorBuffer FVoxelDoubleVectorBuffer::Make(const FVoxelDoubleVector& Value)
{
	FVoxelDoubleVectorBuffer Result;
	Result.X = FVoxelDoubleBuffer::Make(Value.X);
	Result.Y = FVoxelDoubleBuffer::Make(Value.Y);
	Result.Z = FVoxelDoubleBuffer::Make(Value.Z);
	return Result;
}

FVoxelDoubleVectorBuffer FVoxelDoubleVectorBuffer::Make(
	FVoxelDoubleBufferStorage& InX,
	FVoxelDoubleBufferStorage& InY,
	FVoxelDoubleBufferStorage& InZ)
{
	FVoxelDoubleVectorBuffer Result;
	Result.X = FVoxelDoubleBuffer::Make(InX);
	Result.Y = FVoxelDoubleBuffer::Make(InY);
	Result.Z = FVoxelDoubleBuffer::Make(InZ);
	return Result;
}

void FVoxelDoubleVectorBuffer::InitializeFromConstant(const FVoxelRuntimePinValue& Constant)
{
	X = FVoxelDoubleBuffer::Make(Constant.Get<UniformType>().X);
	Y = FVoxelDoubleBuffer::Make(Constant.Get<UniformType>().Y);
	Z = FVoxelDoubleBuffer::Make(Constant.Get<UniformType>().Z);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelDoubleQuaternionBuffer FVoxelDoubleQuaternionBuffer::Make(const FVoxelDoubleQuat& Value)
{
	FVoxelDoubleQuaternionBuffer Result;
	Result.X = FVoxelDoubleBuffer::Make(Value.X);
	Result.Y = FVoxelDoubleBuffer::Make(Value.Y);
	Result.Z = FVoxelDoubleBuffer::Make(Value.Z);
	Result.W = FVoxelDoubleBuffer::Make(Value.W);
	return Result;
}

FVoxelDoubleQuaternionBuffer FVoxelDoubleQuaternionBuffer::Make(
	FVoxelDoubleBufferStorage& InX,
	FVoxelDoubleBufferStorage& InY,
	FVoxelDoubleBufferStorage& InZ,
	FVoxelDoubleBufferStorage& InW)
{
	FVoxelDoubleQuaternionBuffer Result;
	Result.X = FVoxelDoubleBuffer::Make(InX);
	Result.Y = FVoxelDoubleBuffer::Make(InY);
	Result.Z = FVoxelDoubleBuffer::Make(InZ);
	Result.W = FVoxelDoubleBuffer::Make(InW);
	return Result;
}

void FVoxelDoubleQuaternionBuffer::InitializeFromConstant(const FVoxelRuntimePinValue& Constant)
{
	X = FVoxelDoubleBuffer::Make(Constant.Get<UniformType>().X);
	Y = FVoxelDoubleBuffer::Make(Constant.Get<UniformType>().Y);
	Z = FVoxelDoubleBuffer::Make(Constant.Get<UniformType>().Z);
	W = FVoxelDoubleBuffer::Make(Constant.Get<UniformType>().W);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelDoubleTransformBuffer::InitializeFromConstant(const FVoxelRuntimePinValue& Constant)
{
	Rotation.InitializeFromConstant(FVoxelRuntimePinValue::Make(Constant.Get<UniformType>().Rotation));
	Translation.InitializeFromConstant(FVoxelRuntimePinValue::Make(Constant.Get<UniformType>().Translation));
	Scale.InitializeFromConstant(FVoxelRuntimePinValue::Make(Constant.Get<UniformType>().Scale3D));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelDoubleLinearColorBuffer FVoxelDoubleLinearColorBuffer::Make(const FLinearColor& Value)
{
	FVoxelDoubleLinearColorBuffer Result;
	Result.R = FVoxelDoubleBuffer::Make(Value.R);
	Result.G = FVoxelDoubleBuffer::Make(Value.G);
	Result.B = FVoxelDoubleBuffer::Make(Value.B);
	Result.A = FVoxelDoubleBuffer::Make(Value.A);
	return Result;
}

FVoxelDoubleLinearColorBuffer FVoxelDoubleLinearColorBuffer::Make(
	FVoxelDoubleBufferStorage& InR,
	FVoxelDoubleBufferStorage& InG,
	FVoxelDoubleBufferStorage& InB,
	FVoxelDoubleBufferStorage& InA)
{
	FVoxelDoubleLinearColorBuffer Result;
	Result.R = FVoxelDoubleBuffer::Make(InR);
	Result.G = FVoxelDoubleBuffer::Make(InG);
	Result.B = FVoxelDoubleBuffer::Make(InB);
	Result.A = FVoxelDoubleBuffer::Make(InA);
	return Result;
}

void FVoxelDoubleLinearColorBuffer::InitializeFromConstant(const FVoxelRuntimePinValue& Constant)
{
	R = FVoxelDoubleBuffer::Make(Constant.Get<UniformType>().R);
	G = FVoxelDoubleBuffer::Make(Constant.Get<UniformType>().G);
	B = FVoxelDoubleBuffer::Make(Constant.Get<UniformType>().B);
	A = FVoxelDoubleBuffer::Make(Constant.Get<UniformType>().A);
}