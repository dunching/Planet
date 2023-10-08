// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelNoiseNodesImpl.isph"

FORCEINLINE bool LoadBoolImpl(const uniform bool Data[], const uniform bool bConstant, const varying int32 Index)
{
	return bConstant ? Data[0] : Data[Index];
}
FORCEINLINE float LoadFloatImpl(const uniform float Data[], const uniform bool bConstant, const varying int32 Index)
{
	return bConstant ? Data[0] : Data[Index];
}
FORCEINLINE int32 LoadInt32Impl(const uniform int32 Data[], const uniform bool bConstant, const varying int32 Index)
{
	return bConstant ? Data[0] : Data[Index];
}

#define Load_float(Name, Index) LoadFloatImpl(Array ## Name, bConst ## Name, Index)
#define Store_float(Name, Index, Value) Array ## Name[Index] = Value;

#define Load_float2(Name, Index) MakeFloat2(Load_float(Name ## _X, Index), Load_float(Name ## _Y, Index))

#define Input_Type(Name, Type) \
	const uniform Type Array ## Name[], \
	const uniform bool bConst ## Name

#define Output_Type(Name, Type) \
	uniform Type Array ## Name[]

#define Input_float(Name) Input_Type(Name, float)
#define Output_float(Name) Output_Type(Name, float)

#define Input_float2(Name) \
	Input_float(Name ## _X), \
	Input_float(Name ## _Y)

#define Output_float2(Name) \
	Output_float(Name ## _X), \
	Output_float(Name ## _Y)

enum EOctaveType
{
	OctaveType_SmoothPerlin,
	OctaveType_BillowyPerlin,
	OctaveType_RidgedPerlin,

	OctaveType_SmoothCellular,
	OctaveType_BillowyCellular,
	OctaveType_RidgedCellular,
};

struct FOctave
{
	EOctaveType Type;
	bool bStrengthIsConstant;
	float StrengthConstant;
	const float* StrengthArray;
};

export void VoxelNode_AdvancedNoise2D(
	Input_float2(Position),
	Input_float(Amplitude),
	Input_float(FeatureScale),
	Input_float(Lacunarity),
	Input_float(Gain),
	Input_float(CellularJitter),
	const uniform FOctave Octaves[],
	const uniform int32 NumOctaves,
	const uniform int32 InSeed,
	uniform float ReturnValue[],
	const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const varying float Lacunarity = Load_float(Lacunarity, Index);
		const varying float Gain = Load_float(Gain, Index);
		const varying float CellularJitter = Load_float(CellularJitter, Index);

		varying float Sum = 0.f;
		varying float AmplitudeSum = 0.f;

		varying float Amplitude = 1.f;
		varying float2 Position = Load_float2(Position, Index) / Load_float(FeatureScale, Index);
		uniform int32 Seed = InSeed;

		//UNROLL_N(16)
		for (uniform int32 OctaveIndex = 0; OctaveIndex < NumOctaves; OctaveIndex++)
		{
			const uniform FOctave Octave = Octaves[OctaveIndex];

			varying float Noise;
			switch (Octave.Type)
			{
			default: VOXEL_ASSUME(false);
			case OctaveType_SmoothPerlin:
			{
				Noise = GetPerlin2D(Seed, Position);
			}
			break;
			case OctaveType_BillowyPerlin:
			{
				Noise = abs(GetPerlin2D(Seed, Position)) * 2 - 1;
			}
			break;
			case OctaveType_RidgedPerlin:
			{
				Noise = (1 - abs(GetPerlin2D(Seed, Position))) * 2 - 1;
			}
			break;

			case OctaveType_SmoothCellular:
			{
				Noise = GetCellularNoise2D(Seed, Position, CellularJitter);
			}
			break;
			case OctaveType_BillowyCellular:
			{
				Noise = abs(GetCellularNoise2D(Seed, Position, CellularJitter)) * 2 - 1;
			}
			break;
			case OctaveType_RidgedCellular:
			{
				Noise = (1 - abs(GetCellularNoise2D(Seed, Position, CellularJitter))) * 2 - 1;
			}
			break;
			}

			const varying float Strength = Octave.bStrengthIsConstant ? Octave.StrengthConstant : Octave.StrengthArray[Index];

			Sum = Sum + Noise * Strength * Amplitude;
			AmplitudeSum = AmplitudeSum + abs(Strength * Amplitude);
			Amplitude = Amplitude * Gain;
			Position = Position * Lacunarity;
			Seed = (Seed * 196314165) + 907633515;
		}

		const varying float BaseAmplitude = Load_float(Amplitude, Index);
		ReturnValue[Index] = Sum / (AmplitudeSum == 0.f ? 1.f : AmplitudeSum) * BaseAmplitude;
	}
}