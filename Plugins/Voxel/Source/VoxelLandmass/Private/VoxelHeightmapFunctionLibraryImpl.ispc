// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMinimal.isph"

// ReSharper disable CppCStyleCast

export void VoxelHeightmapFunctionLibrary_SampleHeightmap(
	const uniform float ArrayPositionX[],
	const uniform bool bConstPositionX,
	const uniform float ArrayPositionY[],
	const uniform bool bConstPositionY,
	const uniform float BrushScaleXY,
	const uniform float ScaleZ,
	const uniform float OffsetZ,
	const uniform int32 SizeX,
	const uniform int32 SizeY,
	const uniform uint16 Heightmap[],
	const uniform int32 Num,
	uniform float Heights[])
{
	const uniform float InvBrushScaleXY = 1.f / BrushScaleXY;
	const uniform float HalfSizeX = SizeX / 2.f;
	const uniform float HalfSizeY = SizeY / 2.f;
	const uniform float SizeXMinus2 = SizeX - 2.f;
	const uniform float SizeYMinus2 = SizeY - 2.f;

	FOREACH(Index, 0, Num)
	{
		varying float PositionX = bConstPositionX ? ArrayPositionX[0] : ArrayPositionX[Index];
		varying float PositionY = bConstPositionY ? ArrayPositionY[0] : ArrayPositionY[Index];

		PositionX *= InvBrushScaleXY;
		PositionY *= InvBrushScaleXY;

		PositionX += HalfSizeX;
		PositionY += HalfSizeY;

		const varying float MinXf = clamp(floor(PositionX), 0.f, SizeXMinus2);
		const varying float MinYf = clamp(floor(PositionY), 0.f, SizeYMinus2);

		const varying float AlphaX = clamp(PositionX - MinXf, 0.f, 1.f);
		const varying float AlphaY = clamp(PositionY - MinYf, 0.f, 1.f);

		const varying int32 MinX = (int32)MinXf;
		const varying int32 MinY = (int32)MinYf;
		const varying int32 MaxX = MinX + 1;
		const varying int32 MaxY = MinY + 1;

		varying float Height = BilinearInterpolation(
			IGNORE_PERF_WARNING
			Heightmap[MinX + SizeX * MinY],
			IGNORE_PERF_WARNING
			Heightmap[MaxX + SizeX * MinY],
			IGNORE_PERF_WARNING
			Heightmap[MinX + SizeX * MaxY],
			IGNORE_PERF_WARNING
			Heightmap[MaxX + SizeX * MaxY],
			AlphaX,
			AlphaY);

		Height *= ScaleZ;
		Height += OffsetZ;

		Heights[Index] = Height;
	}
}

// X+
// U = Y
// V = -Z

// X-
// U = Y
// V = Z

// Y+
// U = -X
// V = -Z

// Y-
// U = -X
// V = Z

// Z+
// U = Y
// V = X

// Z-
// U = -Y
// V = X

export void VoxelHeightmapFunctionLibrary_GetDistanceToCubemapPlanet(
	const uniform float ArrayPositionX[],
	const uniform bool bConstPositionX,
	const uniform float ArrayPositionY[],
	const uniform bool bConstPositionY,
	const uniform float ArrayPositionZ[],
	const uniform bool bConstPositionZ,
	const uniform double3& PlanetCenter,
	const uniform double PlanetRadius,
	const uniform double MaxHeight,
	const uniform int32 SizeX,
	const uniform int32 SizeY,
	const uniform uint16* const uniform Heightmaps[6],
	const uniform int32 Num,
	uniform float Distances[])
{
	const uniform float HalfSizeX = SizeX / 2.f;
	const uniform float HalfSizeY = SizeY / 2.f;
	const uniform double MaxHeightDivided = MaxHeight / MAX_uint16;

	const uniform float SizeXMinus2 = SizeX - 2.f;
	const uniform float SizeYMinus2 = SizeY - 2.f;

	FOREACH(Index, 0, Num)
	{
		const varying double FullX = (bConstPositionX ? ArrayPositionX[0] : ArrayPositionX[Index]) - PlanetCenter.x;
		const varying double FullY = (bConstPositionY ? ArrayPositionY[0] : ArrayPositionY[Index]) - PlanetCenter.y;
		const varying double FullZ = (bConstPositionZ ? ArrayPositionZ[0] : ArrayPositionZ[Index]) - PlanetCenter.z;

		const varying double Radius = length(MakeDouble3(FullX, FullY, FullZ));
		const varying double InvRadius = 1. / Radius;

		const varying float X = FullX * InvRadius;
		const varying float Y = FullY * InvRadius;
		const varying float Z = FullZ * InvRadius;

		const varying float AbsX = abs(X);
		const varying float AbsY = abs(Y);
		const varying float AbsZ = abs(Z);

		varying int32 FaceToReduce =
			AbsX >= AbsY &&
			AbsX >= AbsZ
			? 0
			: AbsY >= AbsZ
			? 2
			: 4;

		const varying float FaceValue =
			FaceToReduce == 0
			? X
			: FaceToReduce == 2
			? Y
			: Z;

		FaceToReduce += FaceValue > 0 ? 0 : 1;

		varying float SampledHeight;
		uniform int32 UniformFace;
		if (reduce_equal(FaceToReduce, &UniformFace))
		{
			const uniform int32 Face = UniformFace;

			varying float U =
				Face == 2 || Face == 3
				? -X
				: Face == 5
				? -Y
				: Y;

			varying float V =
				Face == 0 || Face == 2
				? -Z
				: Face == 1 || Face == 3
				? Z
				: X;

			const varying float InvFaceValue = 1. / FaceValue;

			U *= InvFaceValue;
			V *= InvFaceValue;

			const varying float SampleX = (U + 1.f) * HalfSizeX;
			const varying float SampleY = (V + 1.f) * HalfSizeY;

			const varying float MinXf = clamp(floor(SampleX), 0.f, SizeXMinus2);
			const varying float MinYf = clamp(floor(SampleY), 0.f, SizeYMinus2);

			const varying float AlphaX = clamp(SampleX - MinXf, 0.f, 1.f);
			const varying float AlphaY = clamp(SampleY - MinYf, 0.f, 1.f);

			const varying int32 MinX = (int32)MinXf;
			const varying int32 MinY = (int32)MinYf;
			const varying int32 MaxX = MinX + 1;
			const varying int32 MaxY = MinY + 1;

			const uniform uint16* uniform Heightmap = Heightmaps[UniformFace];

			SampledHeight = BilinearInterpolation(
				IGNORE_PERF_WARNING
				Heightmap[MinX + SizeX * MinY],
				IGNORE_PERF_WARNING
				Heightmap[MaxX + SizeX * MinY],
				IGNORE_PERF_WARNING
				Heightmap[MinX + SizeX * MaxY],
				IGNORE_PERF_WARNING
				Heightmap[MaxX + SizeX * MaxY],
				AlphaX,
				AlphaY);
		}
		else
		{
			const varying int32 Face = FaceToReduce;

			varying float U =
				Face == 2 || Face == 3
				? -X
				: Face == 5
				? -Y
				: Y;

			varying float V =
				Face == 0 || Face == 2
				? -Z
				: Face == 1 || Face == 3
				? Z
				: X;

			const varying float InvFaceValue = 1. / FaceValue;

			U *= InvFaceValue;
			V *= InvFaceValue;

			const varying float SampleX = (U + 1.f) * HalfSizeX;
			const varying float SampleY = (V + 1.f) * HalfSizeY;

			const varying float MinXf = clamp(floor(SampleX), 0.f, SizeXMinus2);
			const varying float MinYf = clamp(floor(SampleY), 0.f, SizeYMinus2);

			const varying float AlphaX = clamp(SampleX - MinXf, 0.f, 1.f);
			const varying float AlphaY = clamp(SampleY - MinYf, 0.f, 1.f);

			const varying int32 MinX = (int32)MinXf;
			const varying int32 MinY = (int32)MinYf;
			const varying int32 MaxX = MinX + 1;
			const varying int32 MaxY = MinY + 1;

			IGNORE_PERF_WARNING
			const uniform uint16* varying Heightmap = Heightmaps[Face];

			SampledHeight = BilinearInterpolation(
				IGNORE_PERF_WARNING
				Heightmap[MinX + SizeX * MinY],
				IGNORE_PERF_WARNING
				Heightmap[MaxX + SizeX * MinY],
				IGNORE_PERF_WARNING
				Heightmap[MinX + SizeX * MaxY],
				IGNORE_PERF_WARNING
				Heightmap[MaxX + SizeX * MaxY],
				AlphaX,
				AlphaY);
		}

		const varying double Height = PlanetRadius + SampledHeight * MaxHeightDivided;

		Distances[Index] = Radius - Height;
	}
}