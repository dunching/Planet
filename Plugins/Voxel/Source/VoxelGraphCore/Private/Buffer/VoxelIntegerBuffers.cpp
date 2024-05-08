// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Buffer/VoxelIntegerBuffers.h"

FVoxelIntPointBuffer FVoxelIntPointBuffer::Make(const FIntPoint& Value)
{
	FVoxelIntPointBuffer Result;
	Result.X = FVoxelInt32Buffer::Make(Value.X);
	Result.Y = FVoxelInt32Buffer::Make(Value.Y);
	return Result;
}

FVoxelIntPointBuffer FVoxelIntPointBuffer::Make(
	FVoxelInt32BufferStorage& InX,
	FVoxelInt32BufferStorage& InY)
{
	FVoxelIntPointBuffer Result;
	Result.X = FVoxelInt32Buffer::Make(InX);
	Result.Y = FVoxelInt32Buffer::Make(InY);
	return Result;
}

void FVoxelIntPointBuffer::InitializeFromConstant(const FVoxelRuntimePinValue& Constant)
{
	X = FVoxelInt32Buffer::Make(Constant.Get<UniformType>().X);
	Y = FVoxelInt32Buffer::Make(Constant.Get<UniformType>().Y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelIntVectorBuffer FVoxelIntVectorBuffer::Make(const FIntVector& Value)
{
	FVoxelIntVectorBuffer Result;
	Result.X = FVoxelInt32Buffer::Make(Value.X);
	Result.Y = FVoxelInt32Buffer::Make(Value.Y);
	Result.Z = FVoxelInt32Buffer::Make(Value.Z);
	return Result;
}

FVoxelIntVectorBuffer FVoxelIntVectorBuffer::Make(
	FVoxelInt32BufferStorage& InX,
	FVoxelInt32BufferStorage& InY,
	FVoxelInt32BufferStorage& InZ)
{
	FVoxelIntVectorBuffer Result;
	Result.X = FVoxelInt32Buffer::Make(InX);
	Result.Y = FVoxelInt32Buffer::Make(InY);
	Result.Z = FVoxelInt32Buffer::Make(InZ);
	return Result;
}

void FVoxelIntVectorBuffer::InitializeFromConstant(const FVoxelRuntimePinValue& Constant)
{
	X = FVoxelInt32Buffer::Make(Constant.Get<UniformType>().X);
	Y = FVoxelInt32Buffer::Make(Constant.Get<UniformType>().Y);
	Z = FVoxelInt32Buffer::Make(Constant.Get<UniformType>().Z);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelIntVector4Buffer FVoxelIntVector4Buffer::Make(const FIntVector4& Value)
{
	FVoxelIntVector4Buffer Result;
	Result.X = FVoxelInt32Buffer::Make(Value.X);
	Result.Y = FVoxelInt32Buffer::Make(Value.Y);
	Result.Z = FVoxelInt32Buffer::Make(Value.Z);
	Result.W = FVoxelInt32Buffer::Make(Value.W);
	return Result;
}

FVoxelIntVector4Buffer FVoxelIntVector4Buffer::Make(
	FVoxelInt32BufferStorage& InX,
	FVoxelInt32BufferStorage& InY,
	FVoxelInt32BufferStorage& InZ,
	FVoxelInt32BufferStorage& InW)
{
	FVoxelIntVector4Buffer Result;
	Result.X = FVoxelInt32Buffer::Make(InX);
	Result.Y = FVoxelInt32Buffer::Make(InY);
	Result.Z = FVoxelInt32Buffer::Make(InZ);
	Result.W = FVoxelInt32Buffer::Make(InW);
	return Result;
}

void FVoxelIntVector4Buffer::InitializeFromConstant(const FVoxelRuntimePinValue& Constant)
{
	X = FVoxelInt32Buffer::Make(Constant.Get<UniformType>().X);
	Y = FVoxelInt32Buffer::Make(Constant.Get<UniformType>().Y);
	Z = FVoxelInt32Buffer::Make(Constant.Get<UniformType>().Z);
	W = FVoxelInt32Buffer::Make(Constant.Get<UniformType>().W);
}