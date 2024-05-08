// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMinimal.isph"

export void VoxelNode_HeightSplitter_FirstLayer(
	const uniform float Values[],
	const uniform int32 Num,
	const uniform float CurrentHeight,
	const uniform float CurrentFalloff,
	uniform float OutValues[])
{
	FOREACH(Index, 0, Num)
	{
		const varying float Value = Values[Index];

		OutValues[Index] = min(1.f, 1.f - SmoothStep(CurrentHeight - CurrentFalloff, CurrentHeight + CurrentFalloff, Value));
	}
}

export void VoxelNode_HeightSplitter_MiddleLayer(
	const uniform float Values[],
	const uniform int32 Num,
	const uniform float PreviousHeight,
	const uniform float PreviousFalloff,
	const uniform float CurrentHeight,
	const uniform float CurrentFalloff,
	uniform float OutValues[])
{
	FOREACH(Index, 0, Num)
	{
		const varying float Value = Values[Index];

		varying float Strength = 1.f;
		Strength = min(Strength, SmoothStep(PreviousHeight - PreviousFalloff, PreviousHeight + PreviousFalloff, Value));
		Strength = min(Strength, 1.f - SmoothStep(CurrentHeight - CurrentFalloff, CurrentHeight + CurrentFalloff, Value));

		OutValues[Index] = Strength;
	}
}

export void VoxelNode_HeightSplitter_LastLayer(
	const uniform float Values[],
	const uniform int32 Num,
	const uniform float PreviousHeight,
	const uniform float PreviousFalloff,
	uniform float OutValues[])
{
	FOREACH(Index, 0, Num)
	{
		const varying float Value = Values[Index];

		OutValues[Index] = min(1.f, SmoothStep(PreviousHeight - PreviousFalloff, PreviousHeight + PreviousFalloff, Value));
	}
}