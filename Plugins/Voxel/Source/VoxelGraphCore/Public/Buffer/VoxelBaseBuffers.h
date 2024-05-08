// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelBuffer.h"
#include "VoxelBaseBuffers.generated.h"

DECLARE_VOXEL_TERMINAL_BUFFER(FVoxelBoolBuffer, bool);

USTRUCT(DisplayName = "Boolean Buffer")
struct VOXELGRAPHCORE_API FVoxelBoolBuffer final : public FVoxelSimpleTerminalBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_TERMINAL_BUFFER_BODY(FVoxelBoolBuffer, bool);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_TERMINAL_BUFFER(FVoxelByteBuffer, uint8);

template<typename T>
struct TVoxelBufferType<T, typename TEnableIf<TIsEnum<T>::Value>::Type>
{
	using Type = FVoxelByteBuffer;
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelByteBuffer final : public FVoxelSimpleTerminalBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_TERMINAL_BUFFER_BODY(FVoxelByteBuffer, uint8);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_TERMINAL_BUFFER(FVoxelFloatBuffer, float);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelFloatBuffer final : public FVoxelSimpleTerminalBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_TERMINAL_BUFFER_BODY(FVoxelFloatBuffer, float);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_TERMINAL_BUFFER(FVoxelDoubleBuffer, double);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelDoubleBuffer final : public FVoxelSimpleTerminalBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_TERMINAL_BUFFER_BODY(FVoxelDoubleBuffer, double);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_TERMINAL_BUFFER(FVoxelInt32Buffer, int32);

USTRUCT(DisplayName = "Integer Buffer")
struct VOXELGRAPHCORE_API FVoxelInt32Buffer final : public FVoxelSimpleTerminalBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_TERMINAL_BUFFER_BODY(FVoxelInt32Buffer, int32);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_TERMINAL_BUFFER(FVoxelInt64Buffer, int64);

USTRUCT(DisplayName = "Integer64 Buffer")
struct VOXELGRAPHCORE_API FVoxelInt64Buffer final : public FVoxelSimpleTerminalBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_TERMINAL_BUFFER_BODY(FVoxelInt64Buffer, int64);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelSeed
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Seed = 0;

	FVoxelSeed() = default;
	FORCEINLINE FVoxelSeed(const int32 Seed)
		: Seed(Seed)
	{
	}
	FORCEINLINE operator int32() const
	{
		return Seed;
	}

	FORCEINLINE bool operator==(const FVoxelSeed& Other) const
	{
		return Seed == Other.Seed;
	}
	FORCEINLINE friend uint32 GetTypeHash(const FVoxelSeed InSeed)
	{
		return InSeed.Seed;
	}
};
checkStatic(sizeof(FVoxelSeed) == sizeof(int32));

DECLARE_VOXEL_TERMINAL_BUFFER(FVoxelSeedBuffer, FVoxelSeed);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelSeedBuffer final : public FVoxelSimpleTerminalBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_TERMINAL_BUFFER_BODY(FVoxelSeedBuffer, FVoxelSeed);
};