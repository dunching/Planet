// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "MarchingCube/VoxelMarchingCubeNodes.h"

struct VOXELGRAPHNODES_API FVoxelMarchingCubeProcessor
{
	const int32 ChunkSize;
	const int32 DataSize;
	const FVoxelFloatBufferStorage& Distances;

	FVoxelMarchingCubeSurface& Surface;

	bool bPerfectTransitions = false;

	struct FTransitionVertexToQuery
	{
		int32 Index = 0;
		FIntVector PositionA = FIntVector(ForceInit);
		FIntVector PositionB = FIntVector(ForceInit);
	};
	TVoxelStaticArray<TVoxelArray<FTransitionVertexToQuery>, 6> TransitionVerticesToQuery;

	FVoxelMarchingCubeProcessor(
		const int32 ChunkSize,
		const int32 DataSize,
		FVoxelFloatBufferStorage& Distances,
		FVoxelMarchingCubeSurface& Surface);
	~FVoxelMarchingCubeProcessor();

	void Generate(bool bGenerateTransitions);

private:
	TVoxelArray<int32> VertexIndexToCellIndex;
	TVoxelAddOnlyMap<int32, int32> CacheIndexToVertexIndex;

	FORCEINLINE int32 GetIndex(const int32 X, const int32 Y, const int32 Z) const
	{
		return FVoxelUtilities::Get3DIndex<int32>(DataSize, X, Y, Z);
	}
	FORCEINLINE int32 GetCacheIndex(const FIntVector& Position, const int32 EdgeIndex) const
	{
		checkVoxelSlow(0 <= EdgeIndex && EdgeIndex < 3);
		const int32 Index = FVoxelUtilities::Get3DIndex<int32>(ChunkSize + 1, Position);
		return EdgeIndex + 3 * Index;
	}

	void FindCells();
	void ProcessCells();

private:
	FVoxelBitArray32 VerticesToTranslate;
	TVoxelStaticArray<FVoxelBitArray32, 6> TransitionCells;

	template<int32 Direction>
	FORCEINLINE FIntVector GetPosition(const int32 X, const int32 Y) const
	{
		checkVoxelSlow(0 <= X && X <= ChunkSize);
		checkVoxelSlow(0 <= Y && Y <= ChunkSize);

		switch (Direction)
		{
		default: VOXEL_ASSUME(false);
		case 0: return { 0, X, Y };
		case 1: return { ChunkSize, Y, X };
		case 2: return { Y, 0, X };
		case 3: return { X, ChunkSize, Y };
		case 4: return { X, Y, 0 };
		case 5: return { Y, X, ChunkSize };
		}
	}
	template<int32 Direction>
	FORCEINLINE int32 GetIndex(const int32 X, const int32 Y) const
	{
		const FIntVector Position = GetPosition<Direction>(X, Y);
		return GetIndex(Position.X, Position.Y, Position.Z);
	}

	void FindTransitionCells();
	template<int32 Direction>
	void ProcessTransitionCells();
	void ProcessTransitionCells();
};