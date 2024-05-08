// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMinimal.isph"
#include "VoxelSurfaceImpl.isph"

// ReSharper disable CppCStyleCast

export void VoxelSurfaceFunctionLibrary_Invert(
	const uniform float Distance[],
	uniform float OutDistance[],
	const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		OutDistance[Index] = -Distance[Index];
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define StackSize 32

FORCEINLINE void AddLayers(
	const varying int32 Index,
	const varying float Alpha,
	const uniform FVoxelSurfaceLayer Layers[],
	const uniform int32 NumLayers,
	varying int32 Materials[StackSize],
	varying float Strengths[StackSize],
	varying int32& NumWritten)
{
	for (uniform int32 LayerIndex = 0; LayerIndex < NumLayers; LayerIndex++)
	{
		const uniform FVoxelSurfaceLayer uniform& Layer = Layers[LayerIndex];

		const varying uint16 Material = Layer.bConstantMaterials ? Layer.Materials[0] : Layer.Materials[Index];
		const varying uint8 StrengthByte = Layer.bConstantStrengths ? Layer.Strengths[0] : Layer.Strengths[Index];
		const varying float Strength = Alpha * UINT8ToFloat(StrengthByte);
		check(Material != GVoxelInvalidMaterialId || Strength == 0);

		varying bool Mask = Strength > 0;

		if (none(Mask))
		{
			continue;
		}

		const uniform int32 MaxNumWritten = reduce_max(NumWritten);
		for (uniform int32 WriteIndex = 0; WriteIndex < MaxNumWritten; WriteIndex++)
		{
			if (WriteIndex < NumWritten &&
				Materials[WriteIndex] == Material)
			{
				Mask = false;
				Strengths[WriteIndex] += Strength;
			}
		}

		if (!Mask)
		{
			continue;
		}

		ensure(NumWritten < StackSize);
		NumWritten = min(StackSize, NumWritten + 1);

		const varying int32 WriteIndex = NumWritten - 1;
		IGNORE_PERF_WARNING
		Materials[WriteIndex] = Material;
		IGNORE_PERF_WARNING
		Strengths[WriteIndex] = Strength;
	}
}

export void VoxelSurfaceFunctionLibrary_MergeSurfaces(
	const uniform float Alphas[],
	const uniform bool bConstantAlpha,
	const uniform FVoxelSurfaceLayer LayersA[],
	const uniform FVoxelSurfaceLayer LayersB[],
	const uniform int32 NumLayersA,
	const uniform int32 NumLayersB,
	const uniform int32 Num,
	uniform FVoxelSurfaceWriteLayer OutLayers[])
{
	const uniform int32 OutNumLayers = NumLayersA + NumLayersB;
	check(OutNumLayers <= StackSize);

	FOREACH(Index, 0, Num)
	{
		const varying float Alpha = clamp(bConstantAlpha ? Alphas[0] : Alphas[Index], 0.f, 1.f);
		const varying float StrengthA = 1.f - Alpha;
		const varying float StrengthB = Alpha;

		varying int32 Materials[StackSize];
		varying float Strengths[StackSize];
		varying int32 NumWritten = 0;

		AddLayers(Index, StrengthA, LayersA, NumLayersA, Materials, Strengths, NumWritten);
		AddLayers(Index, StrengthB, LayersB, NumLayersB, Materials, Strengths, NumWritten);

		const uniform int32 MaxNumWritten = reduce_max(NumWritten);
		check(MaxNumWritten <= OutNumLayers);

		varying float StrengthSum = 0.f;
		for (uniform int32 WriteIndex = 0; WriteIndex < MaxNumWritten; WriteIndex++)
		{
			if (WriteIndex < NumWritten)
			{
				StrengthSum += Strengths[WriteIndex];
			}
		}

		if (StrengthSum == 0.f)
		{
			StrengthSum = 1.f;
		}

		for (uniform int32 WriteIndex = 0; WriteIndex < OutNumLayers; WriteIndex++)
		{
			OutLayers[WriteIndex].Materials[Index] = WriteIndex < NumWritten ? Materials[WriteIndex] : GVoxelInvalidMaterialId;
			OutLayers[WriteIndex].Strengths[Index] = WriteIndex < NumWritten ? FloatToUINT8(Strengths[WriteIndex] / StrengthSum) : 0;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

export void VoxelSurfaceFunctionLibrary_MakeSphereSurface(
	const uniform float PositionX[],
	const uniform bool bConstantPositionX,
	const uniform float PositionY[],
	const uniform bool bConstantPositionY,
	const uniform float PositionZ[],
	const uniform bool bConstantPositionZ,
	const uniform float3& Center,
	const uniform float Radius,
	uniform float OutDistance[],
	const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const varying float3 Position = MakeFloat3(
			bConstantPositionX ? PositionX[0] : PositionX[Index],
			bConstantPositionY ? PositionY[0] : PositionY[Index],
			bConstantPositionZ ? PositionZ[0] : PositionZ[Index]);

		OutDistance[Index] = length(Position - Center) - Radius;
	}
}

export void VoxelSurfaceFunctionLibrary_MakeBoxSurface(
	const uniform float PositionX[],
	const uniform bool bConstantPositionX,
	const uniform float PositionY[],
	const uniform bool bConstantPositionY,
	const uniform float PositionZ[],
	const uniform bool bConstantPositionZ,
	const uniform float3& Center,
	const uniform float3& Extent,
	const uniform float Smoothness,
	uniform float OutDistance[],
	const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const varying float3 Position = MakeFloat3(
			bConstantPositionX ? PositionX[0] : PositionX[Index],
			bConstantPositionY ? PositionY[0] : PositionY[Index],
			bConstantPositionZ ? PositionZ[0] : PositionZ[Index]);

		const float3 Q = abs(Position - Center) - Extent;
		OutDistance[Index] = length(max(Q, 0.f)) + min(max(Q.x, max(Q.y, Q.z)), 0.f) - Smoothness;
	}
}