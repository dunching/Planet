// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMinimal.isph"

FORCEINLINE float Sample(
	const uniform int3& BrushSize,
	const uniform float DistanceField[],
	const uniform FVoxelOctahedron Normals[],
	const uniform bool bHermiteInterpolation,
	const varying float3 WorldPosition)
{
	const varying float3 ClampedWorldPosition = clamp(WorldPosition, float3_zero, MakeFloat3(BrushSize) - 1.f - 0.0001f);
	const varying float3 Position = floor(ClampedWorldPosition);
	const varying float3 Alpha = ClampedWorldPosition - Position;

	check(0 <= Position.x && Position.x + 1 < BrushSize.x);
	check(0 <= Position.y && Position.y + 1 < BrushSize.y);
	check(0 <= Position.z && Position.z + 1 < BrushSize.z);

	const varying int32 X = Position.x;
	const varying int32 Y = Position.y;
	const varying int32 Z = Position.z;

	IGNORE_PERF_WARNING
	const varying float Distance000 = DistanceField[(X + 0) + BrushSize.x * (Y + 0) + BrushSize.x * BrushSize.y * (Z + 0)];
	IGNORE_PERF_WARNING
	const varying float Distance001 = DistanceField[(X + 1) + BrushSize.x * (Y + 0) + BrushSize.x * BrushSize.y * (Z + 0)];
	IGNORE_PERF_WARNING
	const varying float Distance010 = DistanceField[(X + 0) + BrushSize.x * (Y + 1) + BrushSize.x * BrushSize.y * (Z + 0)];
	IGNORE_PERF_WARNING
	const varying float Distance011 = DistanceField[(X + 1) + BrushSize.x * (Y + 1) + BrushSize.x * BrushSize.y * (Z + 0)];
	IGNORE_PERF_WARNING
	const varying float Distance100 = DistanceField[(X + 0) + BrushSize.x * (Y + 0) + BrushSize.x * BrushSize.y * (Z + 1)];
	IGNORE_PERF_WARNING
	const varying float Distance101 = DistanceField[(X + 1) + BrushSize.x * (Y + 0) + BrushSize.x * BrushSize.y * (Z + 1)];
	IGNORE_PERF_WARNING
	const varying float Distance110 = DistanceField[(X + 0) + BrushSize.x * (Y + 1) + BrushSize.x * BrushSize.y * (Z + 1)];
	IGNORE_PERF_WARNING
	const varying float Distance111 = DistanceField[(X + 1) + BrushSize.x * (Y + 1) + BrushSize.x * BrushSize.y * (Z + 1)];

	varying float BrushDistance;

	if (bHermiteInterpolation)
	{
		IGNORE_PERF_WARNING
		const varying float3 Normal000 = OctahedronToUnitVector(Normals[(X + 0) + BrushSize.x * (Y + 0) + BrushSize.x * BrushSize.y * (Z + 0)]);
		IGNORE_PERF_WARNING
		const varying float3 Normal001 = OctahedronToUnitVector(Normals[(X + 1) + BrushSize.x * (Y + 0) + BrushSize.x * BrushSize.y * (Z + 0)]);
		IGNORE_PERF_WARNING
		const varying float3 Normal010 = OctahedronToUnitVector(Normals[(X + 0) + BrushSize.x * (Y + 1) + BrushSize.x * BrushSize.y * (Z + 0)]);
		IGNORE_PERF_WARNING
		const varying float3 Normal011 = OctahedronToUnitVector(Normals[(X + 1) + BrushSize.x * (Y + 1) + BrushSize.x * BrushSize.y * (Z + 0)]);
		IGNORE_PERF_WARNING
		const varying float3 Normal100 = OctahedronToUnitVector(Normals[(X + 0) + BrushSize.x * (Y + 0) + BrushSize.x * BrushSize.y * (Z + 1)]);
		IGNORE_PERF_WARNING
		const varying float3 Normal101 = OctahedronToUnitVector(Normals[(X + 1) + BrushSize.x * (Y + 0) + BrushSize.x * BrushSize.y * (Z + 1)]);
		IGNORE_PERF_WARNING
		const varying float3 Normal110 = OctahedronToUnitVector(Normals[(X + 0) + BrushSize.x * (Y + 1) + BrushSize.x * BrushSize.y * (Z + 1)]);
		IGNORE_PERF_WARNING
		const varying float3 Normal111 = OctahedronToUnitVector(Normals[(X + 1) + BrushSize.x * (Y + 1) + BrushSize.x * BrushSize.y * (Z + 1)]);

		const varying float P0X = HermiteP0(Alpha.x);
		const varying float P0Y = HermiteP0(Alpha.y);
		const varying float P0Z = HermiteP0(Alpha.z);

		const varying float P1X = HermiteP1(Alpha.x);
		const varying float P1Y = HermiteP1(Alpha.y);
		const varying float P1Z = HermiteP1(Alpha.z);

		const varying float D0X = HermiteD0(Alpha.x);
		const varying float D0Y = HermiteD0(Alpha.y);
		const varying float D0Z = HermiteD0(Alpha.z);

		const varying float D1X = HermiteD1(Alpha.x);
		const varying float D1Y = HermiteD1(Alpha.y);
		const varying float D1Z = HermiteD1(Alpha.z);

		BrushDistance =
			Distance000 * P0X * P0Y * P0Z +
			Normal000.x * D0X * P0Y * P0Z +
			Normal000.y * P0X * D0Y * P0Z +
			Normal000.z * P0X * P0Y * D0Z +

			Distance001 * P1X * P0Y * P0Z +
			Normal001.x * D1X * P0Y * P0Z +
			Normal001.y * P1X * D0Y * P0Z +
			Normal001.z * P1X * P0Y * D0Z +

			Distance010 * P0X * P1Y * P0Z +
			Normal010.x * D0X * P1Y * P0Z +
			Normal010.y * P0X * D1Y * P0Z +
			Normal010.z * P0X * P1Y * D0Z +

			Distance011 * P1X * P1Y * P0Z +
			Normal011.x * D1X * P1Y * P0Z +
			Normal011.y * P1X * D1Y * P0Z +
			Normal011.z * P1X * P1Y * D0Z +

			Distance100 * P0X * P0Y * P1Z +
			Normal100.x * D0X * P0Y * P1Z +
			Normal100.y * P0X * D0Y * P1Z +
			Normal100.z * P0X * P0Y * D1Z +

			Distance101 * P1X * P0Y * P1Z +
			Normal101.x * D1X * P0Y * P1Z +
			Normal101.y * P1X * D0Y * P1Z +
			Normal101.z * P1X * P0Y * D1Z +

			Distance110 * P0X * P1Y * P1Z +
			Normal110.x * D0X * P1Y * P1Z +
			Normal110.y * P0X * D1Y * P1Z +
			Normal110.z * P0X * P1Y * D1Z +

			Distance111 * P1X * P1Y * P1Z +
			Normal111.x * D1X * P1Y * P1Z +
			Normal111.y * P1X * D1Y * P1Z +
			Normal111.z * P1X * P1Y * D1Z;
	}
	else
	{
		BrushDistance = TrilinearInterpolation(
			Distance000,
			Distance001,
			Distance010,
			Distance011,
			Distance100,
			Distance101,
			Distance110,
			Distance111,
			Alpha.x,
			Alpha.y,
			Alpha.z);
	}

	BrushDistance += distance(WorldPosition, ClampedWorldPosition);

	return BrushDistance;
}

export void VoxelVoxelizedMeshFunctionLibrary_SampleVoxelizedMesh(
	const uniform float PositionX[],
	const uniform float PositionY[],
	const uniform float PositionZ[],
	const uniform bool bConstPositionX,
	const uniform bool bConstPositionY,
	const uniform bool bConstPositionZ,
	const uniform int32 Num,
	const uniform int3& BrushSize,
	const uniform float DistanceField[],
	const uniform FVoxelOctahedron Normals[],
	const uniform float3& Origin,
	const uniform float VoxelSize,
	const uniform bool bHermiteInterpolation,
	uniform float DistanceData[])
{
	if (bConstPositionX ||
		bConstPositionY ||
		bConstPositionZ)
	{
		FOREACH(Index, 0, Num)
		{
			const varying float3 WorldPosition = MakeFloat3(
				bConstPositionX ? PositionX[0] : PositionX[Index],
				bConstPositionY ? PositionY[0] : PositionY[Index],
				bConstPositionZ ? PositionZ[0] : PositionZ[Index]) / VoxelSize - Origin;

			DistanceData[Index] = VoxelSize * Sample(
				BrushSize,
				DistanceField,
				Normals,
				bHermiteInterpolation,
				WorldPosition);
		}
		return;
	}

	if (bHermiteInterpolation)
	{
		FOREACH(Index, 0, Num)
		{
			const varying float3 WorldPosition = MakeFloat3(
				PositionX[Index],
				PositionY[Index],
				PositionZ[Index]) / VoxelSize - Origin;

			DistanceData[Index] = VoxelSize * Sample(
				BrushSize,
				DistanceField,
				Normals,
				true,
				WorldPosition);
		}
	}
	else
	{
		FOREACH(Index, 0, Num)
		{
			const varying float3 WorldPosition = MakeFloat3(
				PositionX[Index],
				PositionY[Index],
				PositionZ[Index]) / VoxelSize - Origin;

			DistanceData[Index] = VoxelSize * Sample(
				BrushSize,
				DistanceField,
				Normals,
				false,
				WorldPosition);
		}
	}
}