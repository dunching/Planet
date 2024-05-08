// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelRandomSelectNode.h"
#include "VoxelBufferUtilities.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_RandomSelect, Result)
{
	const TValue<FVoxelSeedBuffer> Seeds = GetNodeRuntime().Get(SeedPin, Query);
	const TValue<FVoxelBuffer> Values = GetNodeRuntime().Get<FVoxelBuffer>(ValuesPin, Query);
	const TValue<FVoxelFloatBuffer> Weights = GetNodeRuntime().Get(WeightsPin, Query);

	return VOXEL_ON_COMPLETE(Seeds, Values, Weights)
	{
		const int32 NumValues = Values->Num();
		if (NumValues == 0)
		{
			return {};
		}

		FVoxelNodeStatScope StatScope(*this, Seeds.Num());

		FVoxelInt32BufferStorage Indices;
		Indices.Allocate(Seeds.Num());

		const int32 Random = STATIC_HASH("RandomSelect");

		if (Weights.Num() > 0)
		{
			if (Weights.Num() != NumValues)
			{
				VOXEL_MESSAGE(Error, "Values.Num={0} but Weights.Num={1}", NumValues, Weights.Num());
				return {};
			}

			TVoxelArray<double> RunningSum;
			FVoxelUtilities::SetNumFast(RunningSum, NumValues);

			double TotalSum = 0.;
			for (int32 Index = 0; Index < NumValues; Index++)
			{
				const float Weight = FMath::Max(Weights[Index], 0.f);
				TotalSum += Weight;

				if (Index == 0)
				{
					RunningSum[Index] = Weight;
				}
				else
				{
					RunningSum[Index] = RunningSum[Index - 1] + Weight;
				}
			}

			for (int32 Index = 0; Index < Seeds.Num(); Index++)
			{
				const FVoxelSeed Seed = Seeds[Index];
				const uint32 LocalSeed = FVoxelUtilities::MurmurHash(uint32(Seed), Random);
				const double Value = TotalSum * FVoxelUtilities::GetFraction(LocalSeed);

				for (int32 ValueIndex = 0; ValueIndex < NumValues; ValueIndex++)
				{
					if (Value <= RunningSum[ValueIndex])
					{
						Indices[Index] = ValueIndex;
						goto End;
					}
				}

				ensureVoxelSlow(false);
				Indices[Index] = 0;

			End:
				;
			}
		}
		else
		{
			for (int32 Index = 0; Index < Seeds.Num(); Index++)
			{
				const FVoxelSeed Seed = Seeds[Index];
				const uint32 LocalSeed = FVoxelUtilities::MurmurHash(uint32(Seed), Random);
				Indices[Index] = FVoxelUtilities::RandRange(LocalSeed, 0, NumValues - 1);
			}
		}

		const TSharedRef<const FVoxelBuffer> Buffer = FVoxelBufferUtilities::Gather(*Values, FVoxelInt32Buffer::Make(Indices));
		return FVoxelRuntimePinValue::Make(Buffer, ReturnPinType);
	};
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_RandomSelect::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == ValuesPin)
	{
		return FVoxelPinTypeSet::AllBufferArrays();
	}
	else
	{
		return FVoxelPinTypeSet::AllBuffers();
	}
}

void FVoxelNode_RandomSelect::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	Pin.SetType(NewType);

	if (Pin.Name == ValuesPin)
	{
		GetPin(ResultPin).SetType(NewType.WithBufferArray(false));
	}
	else
	{
		GetPin(ValuesPin).SetType(NewType.WithBufferArray(true));
	}
}
#endif