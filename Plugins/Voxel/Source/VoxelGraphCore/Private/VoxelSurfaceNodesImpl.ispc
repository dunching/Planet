// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMinimal.isph"

export void VoxelNode_MakeHeightSurface(
	const uniform float Heights[],
	const uniform bool bConstHeight,
	const uniform float PositionZ[],
	const uniform bool bConstPositionZ,
	const uniform int32 Num,
	uniform float OutDistance[])
{
	FOREACH(Index, 0, Num)
	{
		const varying float Height = bConstHeight ? Heights[0] : Heights[Index];
		const varying float Z = bConstPositionZ ? PositionZ[0] : PositionZ[Index];
		OutDistance[Index] = Z - Height;
	}
}

export void VoxelNode_GrowSurface(
	const uniform float Distance[],
	const uniform bool bConstDistance,
	const uniform float Amount[],
	const uniform bool bConstAmount,
	const uniform float MaxAmount,
	const uniform float Scale,
	uniform float OutDistance[],
	const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		OutDistance[Index] = (bConstDistance ? Distance[0] : Distance[Index]) - Scale * clamp(bConstAmount ? Amount[0] : Amount[Index], -MaxAmount, MaxAmount);
	}
}