// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelNoiseNodes.h"

FVoxelComputeValue FVoxelNode_MakeSeeds::CompileCompute(const FName PinName) const
{
	int32 PinIndex = -1;
	for (int32 Index = 0; Index < ResultPins.Num(); Index++)
	{
		if (FName(ResultPins[Index]) == PinName)
		{
			PinIndex = Index;
			break;
		}
	}
	if (!ensure(PinIndex != -1))
	{
		VOXEL_MESSAGE(Error, "{0}: invalid node", this);
		return nullptr;
	}

	return [this, PinIndex](const FVoxelQuery& Query) -> FVoxelFutureValue
	{
		FVoxelPinRef TargetPin = ResultPins[PinIndex];
		VOXEL_SETUP_ON_COMPLETE(TargetPin)

		if (AreTemplatePinsBuffers())
		{
			const TValue<FVoxelSeedBuffer> Seeds = GetNodeRuntime().Get<FVoxelSeedBuffer>(SeedPin, Query);
			return VOXEL_ON_COMPLETE(Seeds, PinIndex)
			{
				FVoxelSeedBufferStorage NewSeeds;
				NewSeeds.Allocate(Seeds.Num());

				for (int32 SeedIndex = 0; SeedIndex < Seeds.Num(); SeedIndex++)
				{
					NewSeeds[SeedIndex] = FVoxelUtilities::MurmurHash(Seeds[SeedIndex], PinIndex);
				}

				return FVoxelRuntimePinValue::Make(FVoxelSeedBuffer::Make(NewSeeds));
			};
		}
		else
		{
			const TValue<FVoxelSeed> Seed = GetNodeRuntime().Get<FVoxelSeed>(SeedPin, Query);
			return VOXEL_ON_COMPLETE(Seed, PinIndex)
			{
				const FVoxelSeed NewSeed = FVoxelUtilities::MurmurHash(Seed, PinIndex);
				return FVoxelRuntimePinValue::Make(NewSeed);
			};
		}
	};
}

void FVoxelNode_MakeSeeds::FixupSeedPins()
{
	for (const FVoxelPinRef& ResultPin : ResultPins)
	{
		RemovePin(ResultPin);
	}

	ResultPins.Reset();

	const bool bIsBuffer = AreTemplatePinsBuffers();
	for (int32 Index = 0; Index < NumNewSeeds; Index++)
	{
		if (bIsBuffer)
		{
			ResultPins.Add(
				CreateOutputPin<FVoxelSeedBuffer>(
					FName("Seed", Index + 1),
					VOXEL_PIN_METADATA(
						FVoxelSeedBuffer,
						nullptr,
						DisplayName("Seed " + LexToString(Index + 1))),
					EVoxelPinFlags::TemplatePin)
			);
		}
		else
		{
			ResultPins.Add(
				CreateOutputPin<FVoxelSeed>(
					FName("Seed", Index + 1),
					VOXEL_PIN_METADATA(
						FVoxelSeed,
						nullptr,
						DisplayName("Seed " + LexToString(Index + 1))),
					EVoxelPinFlags::TemplatePin)
			);
		}
	}
}