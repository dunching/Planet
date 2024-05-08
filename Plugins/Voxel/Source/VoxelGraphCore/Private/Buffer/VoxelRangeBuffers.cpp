// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Buffer/VoxelRangeBuffers.h"

FVoxelFloatRangeBuffer FVoxelFloatRangeBuffer::Make(const FVoxelFloatRange& Value)
{
	FVoxelFloatRangeBuffer Result;
	Result.Min = FVoxelFloatBuffer::Make(Value.Min);
	Result.Max = FVoxelFloatBuffer::Make(Value.Max);
	return Result;
}

FVoxelFloatRangeBuffer FVoxelFloatRangeBuffer::Make(
	FVoxelFloatBufferStorage& InMin,
	FVoxelFloatBufferStorage& InMax)
{
	FVoxelFloatRangeBuffer Result;
	Result.Min = FVoxelFloatBuffer::Make(InMin);
	Result.Max = FVoxelFloatBuffer::Make(InMax);
	return Result;
}

void FVoxelFloatRangeBuffer::InitializeFromConstant(const FVoxelRuntimePinValue& Constant)
{
	Min = FVoxelFloatBuffer::Make(Constant.Get<UniformType>().Min);
	Max = FVoxelFloatBuffer::Make(Constant.Get<UniformType>().Max);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelInt32RangeBuffer FVoxelInt32RangeBuffer::Make(const FVoxelInt32Range& Value)
{
	FVoxelInt32RangeBuffer Result;
	Result.Min = FVoxelInt32Buffer::Make(Value.Min);
	Result.Max = FVoxelInt32Buffer::Make(Value.Max);
	return Result;
}

FVoxelInt32RangeBuffer FVoxelInt32RangeBuffer::Make(
	FVoxelInt32BufferStorage& InMin,
	FVoxelInt32BufferStorage& InMax)
{
	FVoxelInt32RangeBuffer Result;
	Result.Min = FVoxelInt32Buffer::Make(InMin);
	Result.Max = FVoxelInt32Buffer::Make(InMax);
	return Result;
}

void FVoxelInt32RangeBuffer::InitializeFromConstant(const FVoxelRuntimePinValue& Constant)
{
	Min = FVoxelInt32Buffer::Make(Constant.Get<UniformType>().Min);
	Max = FVoxelInt32Buffer::Make(Constant.Get<UniformType>().Max);
}