// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMinimal.isph"

export void VoxelMathFunctionLibrary_SmoothMin(
	const uniform float DistancesA[],
	const uniform bool bConstDistanceA,
	const uniform float DistancesB[],
	const uniform bool bConstDistanceB,
	const uniform float Smoothnesses[],
	const uniform bool bConstSmoothness,
	const uniform int32 Num,
	uniform float OutAlpha[],
	uniform float OutDistance[])
{
	FOREACH(Index, 0, Num)
	{
		const varying float DistanceA = bConstDistanceA ? DistancesA[0] : DistancesA[Index];
		const varying float DistanceB = bConstDistanceB ? DistancesB[0] : DistancesB[Index];
		const varying float Smoothness = bConstSmoothness ? Smoothnesses[0] : Smoothnesses[Index];

		const varying float OneMinusAlpha = clamp(0.5f + 0.5f * SmoothHelper(DistanceB - DistanceA, Smoothness), 0.0f, 1.0f);
		const varying float Alpha = 1.f - OneMinusAlpha;

		OutAlpha[Index] = Alpha;
		OutDistance[Index] = lerp(DistanceA, DistanceB, Alpha) - Smoothness * Alpha * OneMinusAlpha;
	}
}

export void VoxelMathFunctionLibrary_SmoothMax(
	const uniform float DistancesA[],
	const uniform bool bConstDistanceA,
	const uniform float DistancesB[],
	const uniform bool bConstDistanceB,
	const uniform float Smoothnesses[],
	const uniform bool bConstSmoothness,
	const uniform int32 Num,
	uniform float OutAlpha[],
	uniform float OutDistance[])
{
	FOREACH(Index, 0, Num)
	{
		const varying float DistanceA = bConstDistanceA ? DistancesA[0] : DistancesA[Index];
		const varying float DistanceB = bConstDistanceB ? DistancesB[0] : DistancesB[Index];
		const varying float Smoothness = bConstSmoothness ? Smoothnesses[0] : Smoothnesses[Index];

		const varying float OneMinusAlpha = clamp(0.5f - 0.5f * SmoothHelper(DistanceB - DistanceA, Smoothness), 0.0f, 1.0f);
		const varying float Alpha = 1.f - OneMinusAlpha;

		OutAlpha[Index] = Alpha;
		OutDistance[Index] = lerp(DistanceA, DistanceB, Alpha) + Smoothness * Alpha * OneMinusAlpha;
	}
}