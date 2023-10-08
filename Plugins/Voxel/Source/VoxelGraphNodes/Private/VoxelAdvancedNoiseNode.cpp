// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelAdvancedNoiseNode.h"
#include "VoxelAdvancedNoiseNodesImpl.ispc.generated.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_AdvancedNoise2D, Value)
{
	const auto GetISPCNoise = [](EVoxelAdvancedNoiseOctaveType Noise)
	{
		switch (Noise)
		{
		default: ensure(false);

#define CASE(Name) case EVoxelAdvancedNoiseOctaveType::Name: return ispc::OctaveType_ ## Name;

		CASE(SmoothPerlin);
		CASE(BillowyPerlin);
		CASE(RidgedPerlin);

		CASE(SmoothCellular);
		CASE(BillowyCellular);
		CASE(RidgedCellular);

#undef CASE
		}
	};

	const TValue<FVoxelVector2DBuffer> Positions = Get(PositionPin, Query);
	const TValue<FVoxelFloatBuffer> Amplitudes = Get(AmplitudePin, Query);
	const TValue<FVoxelFloatBuffer> FeatureScales = Get(FeatureScalePin, Query);
	const TValue<FVoxelFloatBuffer> Lacunarities = Get(LacunarityPin, Query);
	const TValue<FVoxelFloatBuffer> Gains = Get(GainPin, Query);
	const TValue<FVoxelFloatBuffer> CellularJitters = Get(CellularJitterPin, Query);
	const TValue<int32> NumOctaves = Get(NumOctavesPin, Query);
	const TValue<FVoxelSeed> Seed = Get(SeedPin, Query);
	const TValue<EVoxelAdvancedNoiseOctaveType> DefaultOctaveType = Get(DefaultOctaveTypePin, Query);
	const TVoxelArray<TValue<EVoxelAdvancedNoiseOctaveType>> OctaveTypes = Get(OctaveTypePins, Query);
	const TVoxelArray<TValue<FVoxelFloatBuffer>> OctaveStrengths = Get(OctaveStrengthPins, Query);

	return VOXEL_ON_COMPLETE(Positions, Amplitudes, FeatureScales, Lacunarities, Gains, CellularJitters, NumOctaves, Seed, DefaultOctaveType, OctaveTypes, OctaveStrengths, GetISPCNoise)
	{
		const int32 Num = ComputeVoxelBuffersNum(Positions, Amplitudes, FeatureScales, Lacunarities, Gains, CellularJitters);
		const int32 SafeNumOctaves = FMath::Clamp(NumOctaves, 1, 255);

		TVoxelArray<ispc::FOctave, TVoxelInlineAllocator<16>> OctavesInit;
		OctavesInit.Reserve(SafeNumOctaves);

		for (int32 Index = 0; Index < SafeNumOctaves; Index++)
		{
			ispc::FOctave& Octave = OctavesInit.Emplace_GetRef(ispc::FOctave{});

			if (OctaveTypes.IsValidIndex(Index))
			{
				Octave.Type = GetISPCNoise(OctaveTypes[Index]);
			}
			else
			{
				Octave.Type = GetISPCNoise(DefaultOctaveType);
			}
		}

		VOXEL_SCOPE_COUNTER_FORMAT("AdvancedNoise2D Num=%d", Num);
		FVoxelNodeStatScope StatScope(*this, Num);

		FVoxelFloatBufferStorage ReturnValue;
		ReturnValue.Allocate(Num);

		ForeachVoxelBufferChunk(Num, [&](const FVoxelBufferIterator& Iterator)
		{
			TVoxelArray<ispc::FOctave, TVoxelInlineAllocator<16>> Octaves = OctavesInit;
			for (int32 Index = 0; Index < SafeNumOctaves; Index++)
			{
				ispc::FOctave& Octave = Octaves[Index];

				if (OctaveStrengths.IsValidIndex(Index))
				{
					const FVoxelFloatBuffer& Strength = OctaveStrengths[Index];

					if (Strength.IsConstant())
					{
						Octave.bStrengthIsConstant = true;
						Octave.StrengthConstant = Strength.GetConstant();
					}
					else
					{
						if (Strength.Num() != Num)
						{
							RaiseBufferError(*this);
							return;
						}

						Octave.bStrengthIsConstant = false;
						Octave.StrengthArray = Strength.GetData(Iterator);
					}
				}
				else
				{
					Octave.bStrengthIsConstant = true;
					Octave.StrengthConstant = 1.f;
					Octave.StrengthArray = nullptr;
				}
			}

			ispc::VoxelNode_AdvancedNoise2D(
				Positions.X.GetData(Iterator),
				Positions.X.IsConstant(),
				Positions.Y.GetData(Iterator),
				Positions.Y.IsConstant(),
				Amplitudes.GetData(Iterator),
				Amplitudes.IsConstant(),
				FeatureScales.GetData(Iterator),
				FeatureScales.IsConstant(),
				Lacunarities.GetData(Iterator),
				Lacunarities.IsConstant(),
				Gains.GetData(Iterator),
				Gains.IsConstant(),
				CellularJitters.GetData(Iterator),
				CellularJitters.IsConstant(),
				Octaves.GetData(),
				Octaves.Num(),
				Seed,
				ReturnValue.GetData(Iterator),
				Iterator.Num());
		});

		return FVoxelFloatBuffer::Make(ReturnValue);
	};
}