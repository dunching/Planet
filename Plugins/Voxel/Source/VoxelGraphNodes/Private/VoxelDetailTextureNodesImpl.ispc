// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMinimal.isph"
#include "VoxelSurfaceImpl.isph"

// ReSharper disable CppCStyleCast

export void VoxelNode_MakeMaterialIdDetailTextureParameter_MakePackedMaterial(
	const uniform FVoxelSurfaceLayer Layers[],
	const uniform int32 NumLayers,
	const uniform int32 Num,
	uniform int64 OutPackedMaterials[])
{
	FOREACH(Index, 0, Num)
	{
#define StackSize 32
		varying int32 Materials[StackSize];
		varying int32 Strengths[StackSize];
		uniform int32 NumWritten = 0;

		for (uniform int32 LayerIndex = 0; LayerIndex < NumLayers; LayerIndex++)
		{
			const uniform FVoxelSurfaceLayer uniform& Layer = Layers[LayerIndex];

			const varying uint16 Material = Layer.bConstantMaterials ? Layer.Materials[0] : Layer.Materials[Index];
			const varying uint8 Strength = Layer.bConstantStrengths ? Layer.Strengths[0] : Layer.Strengths[Index];
			check(Material != GVoxelInvalidMaterialId || Strength == 0);

			varying bool Mask = Strength > 0;

			if (none(Mask))
			{
				continue;
			}

			for (uniform int32 WriteIndex = 0; WriteIndex < NumWritten; WriteIndex++)
			{
				if (Materials[WriteIndex] == Material)
				{
					Mask = false;
					Strengths[WriteIndex] += Strength;
				}
			}

			if (none(Mask))
			{
				continue;
			}

			ensure(NumWritten < StackSize);
			NumWritten = min(StackSize, NumWritten + 1);

			const uniform int32 WriteIndex = NumWritten - 1;
			Materials[WriteIndex] = Mask ? Material : GVoxelInvalidMaterialId;
			Strengths[WriteIndex] = Mask ? Strength : 0;
		}

		varying int32 BestMaterialA = 0;
		varying int32 BestMaterialB = 0;
		varying int32 BestMaterialC = 0;
		varying int32 BestStrengthA = 0;
		varying int32 BestStrengthB = 0;
		varying int32 BestStrengthC = 0;

		for (uniform int32 WriteIndex = 0; WriteIndex < NumWritten; WriteIndex++)
		{
			const varying int32 Material = Materials[WriteIndex];
			const varying int32 Strength = Strengths[WriteIndex];

			if (Strength > BestStrengthA)
			{
				BestMaterialC = BestMaterialB;
				BestStrengthC = BestStrengthB;

				BestMaterialB = BestMaterialA;
				BestStrengthB = BestStrengthA;

				BestMaterialA = Material;
				BestStrengthA = Strength;

				continue;
			}

			if (Strength > BestStrengthB)
			{
				BestMaterialC = BestMaterialB;
				BestStrengthC = BestStrengthB;

				BestMaterialB = Material;
				BestStrengthB = Strength;

				continue;
			}

			if (Strength > BestStrengthC)
			{
				BestMaterialC = Material;
				BestStrengthC = Strength;
			}
		}

		const varying float AlphaSum = BestStrengthA + BestStrengthB + BestStrengthC;

		const varying float AlphaA = (float)BestStrengthA / AlphaSum;
		const varying float AlphaB = (float)BestStrengthB / AlphaSum;
		const varying float AlphaC = (float)BestStrengthC / AlphaSum;

		const varying uint8 ByteAlphaA = FloatToUINT8(AlphaA);
		const varying uint8 ByteAlphaB = FloatToUINT8(AlphaB);
		const varying uint8 ByteAlphaC = FloatToUINT8(AlphaC);

		const uint32 DataA =
			ByteAlphaA |
			((uint32)BestMaterialB << 8) |
			((uint32)BestMaterialA << 20);

		const uint32 DataB =
			BestMaterialC |
			((uint32)ByteAlphaC << 16) |
			((uint32)ByteAlphaB << 24);

		OutPackedMaterials[Index] = ((uint64)DataB << 32) | DataA;
#undef StackSize
	}
}