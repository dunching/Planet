// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Buffer/VoxelFloatBuffers.h"

FVoxelVector2DBuffer FVoxelVector2DBuffer::Make(const FVector2f& Value)
{
	FVoxelVector2DBuffer Result;
	Result.X = FVoxelFloatBuffer::Make(Value.X);
	Result.Y = FVoxelFloatBuffer::Make(Value.Y);
	return Result;
}

FVoxelVector2DBuffer FVoxelVector2DBuffer::Make(
	FVoxelFloatBufferStorage& InX,
	FVoxelFloatBufferStorage& InY)
{
	FVoxelVector2DBuffer Result;
	Result.X = FVoxelFloatBuffer::Make(InX);
	Result.Y = FVoxelFloatBuffer::Make(InY);
	return Result;
}

void FVoxelVector2DBuffer::InitializeFromConstant(const FVoxelRuntimePinValue& Constant)
{
	X = FVoxelFloatBuffer::Make(Constant.Get<UniformType>().X);
	Y = FVoxelFloatBuffer::Make(Constant.Get<UniformType>().Y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelVectorBuffer FVoxelVectorBuffer::Make(const FVector3f& Value)
{
	FVoxelVectorBuffer Result;
	Result.X = FVoxelFloatBuffer::Make(Value.X);
	Result.Y = FVoxelFloatBuffer::Make(Value.Y);
	Result.Z = FVoxelFloatBuffer::Make(Value.Z);
	return Result;
}

FVoxelVectorBuffer FVoxelVectorBuffer::Make(
	FVoxelFloatBufferStorage& InX,
	FVoxelFloatBufferStorage& InY,
	FVoxelFloatBufferStorage& InZ)
{
	FVoxelVectorBuffer Result;
	Result.X = FVoxelFloatBuffer::Make(InX);
	Result.Y = FVoxelFloatBuffer::Make(InY);
	Result.Z = FVoxelFloatBuffer::Make(InZ);
	return Result;
}

void FVoxelVectorBuffer::InitializeFromConstant(const FVoxelRuntimePinValue& Constant)
{
	X = FVoxelFloatBuffer::Make(Constant.Get<UniformType>().X);
	Y = FVoxelFloatBuffer::Make(Constant.Get<UniformType>().Y);
	Z = FVoxelFloatBuffer::Make(Constant.Get<UniformType>().Z);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelLinearColorBuffer FVoxelLinearColorBuffer::Make(const FLinearColor& Value)
{
	FVoxelLinearColorBuffer Result;
	Result.R = FVoxelFloatBuffer::Make(Value.R);
	Result.G = FVoxelFloatBuffer::Make(Value.G);
	Result.B = FVoxelFloatBuffer::Make(Value.B);
	Result.A = FVoxelFloatBuffer::Make(Value.A);
	return Result;
}

FVoxelLinearColorBuffer FVoxelLinearColorBuffer::Make(
	FVoxelFloatBufferStorage& InR,
	FVoxelFloatBufferStorage& InG,
	FVoxelFloatBufferStorage& InB,
	FVoxelFloatBufferStorage& InA)
{
	FVoxelLinearColorBuffer Result;
	Result.R = FVoxelFloatBuffer::Make(InR);
	Result.G = FVoxelFloatBuffer::Make(InG);
	Result.B = FVoxelFloatBuffer::Make(InB);
	Result.A = FVoxelFloatBuffer::Make(InA);
	return Result;
}

void FVoxelLinearColorBuffer::InitializeFromConstant(const FVoxelRuntimePinValue& Constant)
{
	R = FVoxelFloatBuffer::Make(Constant.Get<UniformType>().R);
	G = FVoxelFloatBuffer::Make(Constant.Get<UniformType>().G);
	B = FVoxelFloatBuffer::Make(Constant.Get<UniformType>().B);
	A = FVoxelFloatBuffer::Make(Constant.Get<UniformType>().A);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelQuaternionBuffer FVoxelQuaternionBuffer::Make(const FQuat4f& Value)
{
	FVoxelQuaternionBuffer Result;
	Result.X = FVoxelFloatBuffer::Make(Value.X);
	Result.Y = FVoxelFloatBuffer::Make(Value.Y);
	Result.Z = FVoxelFloatBuffer::Make(Value.Z);
	Result.W = FVoxelFloatBuffer::Make(Value.W);
	return Result;
}

FVoxelQuaternionBuffer FVoxelQuaternionBuffer::Make(
	FVoxelFloatBufferStorage& InX,
	FVoxelFloatBufferStorage& InY,
	FVoxelFloatBufferStorage& InZ,
	FVoxelFloatBufferStorage& InW)
{
	FVoxelQuaternionBuffer Result;
	Result.X = FVoxelFloatBuffer::Make(InX);
	Result.Y = FVoxelFloatBuffer::Make(InY);
	Result.Z = FVoxelFloatBuffer::Make(InZ);
	Result.W = FVoxelFloatBuffer::Make(InW);
	return Result;
}

void FVoxelQuaternionBuffer::InitializeFromConstant(const FVoxelRuntimePinValue& Constant)
{
	X = FVoxelFloatBuffer::Make(Constant.Get<UniformType>().X);
	Y = FVoxelFloatBuffer::Make(Constant.Get<UniformType>().Y);
	Z = FVoxelFloatBuffer::Make(Constant.Get<UniformType>().Z);
	W = FVoxelFloatBuffer::Make(Constant.Get<UniformType>().W);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelTransformBuffer::InitializeFromConstant(const FVoxelRuntimePinValue& Constant)
{
	Rotation.InitializeFromConstant(FVoxelRuntimePinValue::Make(Constant.Get<UniformType>().GetRotation()));
	Translation.InitializeFromConstant(FVoxelRuntimePinValue::Make(Constant.Get<UniformType>().GetTranslation()));
	Scale.InitializeFromConstant(FVoxelRuntimePinValue::Make(Constant.Get<UniformType>().GetScale3D()));
}