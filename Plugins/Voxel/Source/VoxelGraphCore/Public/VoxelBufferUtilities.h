// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "Buffer/VoxelFloatBuffers.h"

struct FVoxelPointIdBuffer;

struct VOXELGRAPHCORE_API FVoxelBufferUtilities
{
public:
	static FVoxelVectorBuffer ApplyTransform(const FVoxelVectorBuffer& Buffer, const FTransform& Transform);
	static FVoxelVectorBuffer ApplyInverseTransform(const FVoxelVectorBuffer& Buffer, const FTransform& Transform);

	static FVoxelVectorBuffer ApplyTransform(
		const FVoxelVectorBuffer& Buffer,
		const FVoxelVectorBuffer& Translation,
		const FVoxelQuaternionBuffer& Rotation,
		const FVoxelVectorBuffer& Scale);

	static FVoxelVectorBuffer ApplyTransform(const FVoxelVectorBuffer& Buffer, const FMatrix& Transform);
	static FVoxelFloatBuffer TransformDistance(const FVoxelFloatBuffer& Distance, const FMatrix& Transform);

public:
	static FVoxelFloatBuffer Min8(
		const FVoxelFloatBuffer& Buffer0,
		const FVoxelFloatBuffer& Buffer1,
		const FVoxelFloatBuffer& Buffer2,
		const FVoxelFloatBuffer& Buffer3,
		const FVoxelFloatBuffer& Buffer4,
		const FVoxelFloatBuffer& Buffer5,
		const FVoxelFloatBuffer& Buffer6,
		const FVoxelFloatBuffer& Buffer7);

	static FVoxelFloatBuffer Max8(
		const FVoxelFloatBuffer& Buffer0,
		const FVoxelFloatBuffer& Buffer1,
		const FVoxelFloatBuffer& Buffer2,
		const FVoxelFloatBuffer& Buffer3,
		const FVoxelFloatBuffer& Buffer4,
		const FVoxelFloatBuffer& Buffer5,
		const FVoxelFloatBuffer& Buffer6,
		const FVoxelFloatBuffer& Buffer7);

	static FVoxelVectorBuffer Min8(
		const FVoxelVectorBuffer& Buffer0,
		const FVoxelVectorBuffer& Buffer1,
		const FVoxelVectorBuffer& Buffer2,
		const FVoxelVectorBuffer& Buffer3,
		const FVoxelVectorBuffer& Buffer4,
		const FVoxelVectorBuffer& Buffer5,
		const FVoxelVectorBuffer& Buffer6,
		const FVoxelVectorBuffer& Buffer7);

	static FVoxelVectorBuffer Max8(
		const FVoxelVectorBuffer& Buffer0,
		const FVoxelVectorBuffer& Buffer1,
		const FVoxelVectorBuffer& Buffer2,
		const FVoxelVectorBuffer& Buffer3,
		const FVoxelVectorBuffer& Buffer4,
		const FVoxelVectorBuffer& Buffer5,
		const FVoxelVectorBuffer& Buffer6,
		const FVoxelVectorBuffer& Buffer7);

public:
	static FVoxelFloatBuffer IntToFloat(const FVoxelInt32Buffer& Buffer);
	static FVoxelDoubleBuffer IntToDouble(const FVoxelInt32Buffer& Buffer);

	static FVoxelDoubleBuffer FloatToDouble(const FVoxelFloatBuffer& Float);
	static FVoxelFloatBuffer DoubleToFloat(const FVoxelDoubleBuffer& Double);

	static FVoxelSeedBuffer PointIdToSeed(const FVoxelPointIdBuffer& Buffer);

public:
	static FVoxelFloatBuffer Add(const FVoxelFloatBuffer& A, const FVoxelFloatBuffer& B);
	static FVoxelFloatBuffer Multiply(const FVoxelFloatBuffer& A, const FVoxelFloatBuffer& B);
	static FVoxelBoolBuffer Less(const FVoxelFloatBuffer& A, const FVoxelFloatBuffer& B);

	static FVoxelVectorBuffer Add(const FVoxelVectorBuffer& A, const FVoxelVectorBuffer& B);
	static FVoxelVectorBuffer Multiply(const FVoxelVectorBuffer& A, const FVoxelVectorBuffer& B);

	static FVoxelFloatBuffer Lerp(const FVoxelFloatBuffer& A, const FVoxelFloatBuffer& B, const FVoxelFloatBuffer& Alpha);
	static FVoxelVector2DBuffer Lerp(const FVoxelVector2DBuffer& A, const FVoxelVector2DBuffer& B, const FVoxelFloatBuffer& Alpha);
	static FVoxelVectorBuffer Lerp(const FVoxelVectorBuffer& A, const FVoxelVectorBuffer& B, const FVoxelFloatBuffer& Alpha);
	static FVoxelLinearColorBuffer Lerp(const FVoxelLinearColorBuffer& A, const FVoxelLinearColorBuffer& B, const FVoxelFloatBuffer& Alpha);

	static FVoxelQuaternionBuffer Combine(const FVoxelQuaternionBuffer& A, const FVoxelQuaternionBuffer& B);

public:
	static void MakePalette(
		const FVoxelTerminalBuffer& Buffer,
		FVoxelInt32Buffer& OutIndices,
		FVoxelTerminalBuffer& OutPalette);

public:
	static void Select(
		FVoxelTerminalBuffer& OutBuffer,
		const FVoxelBuffer& Indices,
		TConstVoxelArrayView<const FVoxelTerminalBuffer*> Buffers);

	static TSharedRef<const FVoxelBuffer> Select(
		const FVoxelPinType& InnerType,
		const FVoxelBuffer& Indices,
		TConstVoxelArrayView<const FVoxelBuffer*> Buffers);

public:
	static void Gather(
		FVoxelTerminalBuffer& OutBuffer,
		const FVoxelTerminalBuffer& Buffer,
		const FVoxelInt32Buffer& Indices);

	static TSharedRef<const FVoxelBuffer> Gather(
		const FVoxelBuffer& Buffer,
		const FVoxelInt32Buffer& Indices);

public:
	static void Replicate(
		FVoxelTerminalBuffer& OutBuffer,
		const FVoxelTerminalBuffer& Buffer,
		TConstVoxelArrayView<int32> Counts,
		int32 NewNum);

	static TSharedRef<const FVoxelBuffer> Replicate(
		const FVoxelBuffer& Buffer,
		TConstVoxelArrayView<int32> Counts,
		int32 NewNum);
};