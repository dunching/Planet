// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMinimal.isph"

// ReSharper disable CppCStyleCast

export void VoxelPositionQueryParameter_WritePositions3D(
	uniform float DataX[],
	uniform float DataY[],
	uniform float DataZ[],
	const uniform float StartX,
	const uniform float StartY,
	const uniform float StartZ,
	const uniform int32 SizeX,
	const uniform int32 SizeY,
	const uniform float Step,
	const uniform int32 StartIndex,
	const uniform int32 Num)
{
	const uniform int32 SizeXY = SizeX * SizeY;

	FOREACH(It, 0, Num)
	{
		varying int32 Index = StartIndex + It;

		IGNORE_PERF_WARNING
		const varying int32 Z = Index / SizeXY;
		Index -= Z * SizeXY;

		IGNORE_PERF_WARNING
		const varying int32 Y = Index / SizeX;
		Index -= Y * SizeX;

		const varying int32 X = Index;

		DataX[It] = StartX + X * Step;
		DataY[It] = StartY + Y * Step;
		DataZ[It] = StartZ + Z * Step;
	}
}