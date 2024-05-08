// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"

struct VOXELCORE_API FVoxelDistanceFieldUtilities
{
public:
	FORCEINLINE static bool IsSurfacePositionValid(const FVector3f& P)
	{
		return P.X < 1e9;
	}
	FORCEINLINE static FVector3f MakeInvalidSurfacePosition()
	{
		return FVector3f(1e9);
	}

public:
	// ~10ms for 32x32x32 on a single 5950X core
	static void JumpFlood(
		const FIntVector& Size,
		TVoxelArray<FVector3f>& InOutPackedPositions,
		bool bParallel = false,
		const FVoxelIntBox* Bounds = nullptr);

	// Only the InOutDistances sign will be used, not their actual values
	static void GetDistancesFromSurfacePositions(
		const FIntVector& Size,
		TVoxelArrayView<const FVector3f> SurfacePositions,
		TVoxelArrayView<float> InOutDistances,
		const FVoxelIntBox* Bounds = nullptr);

public:
	// OutDistances will only have the signs of the values
	// Note: densities need to match Size + 2, so that all neighbors can be queried!
	// OutBounds: bounds of the surface positions, within [0, Size]
	static void GetSurfacePositionsFromDensities(
		const FIntVector& Size,
		TVoxelArrayView<const float> Densities,
		TVoxelArrayView<float> OutDistances,
		TVoxelArrayView<FVector3f> OutSurfacePositions,
		bool bParallel,
		FVoxelOptionalIntBox* OutBounds = nullptr);

	static void GetSurfacePositionsFromDensities(
		const FIntVector& Size,
		const TArray<float>& Densities,
		TArray<float>& OutDistances,
		TArray<FVector3f>& OutSurfacePositions,
		bool bParallel,
		FVoxelOptionalIntBox* OutBounds = nullptr);

private:
	static void JumpFloodStep_CPU(
		const FIntVector& Size,
		const FVoxelIntBox& Bounds,
		TVoxelArrayView<const FVector3f> InData,
		TVoxelArrayView<FVector3f> OutData,
		int32 Step,
		bool bParallel);
};