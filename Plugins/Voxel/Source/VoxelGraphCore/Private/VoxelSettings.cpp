// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelSettings.h"
#include "VoxelChannel.h"
#include "VoxelSurface.h"

void UVoxelSettings::UpdateChannels()
{
	VOXEL_FUNCTION_COUNTER();

	if (GlobalChannels.Num() == 0)
	{
		GlobalChannels.Add(
		{
			"Surface",
			FVoxelPinType::Make<FVoxelSurface>(),
			FVoxelPinValue::Make(FVoxelSurface())
		});
	}

	TSet<FName> UsedNames;
	for (FVoxelChannelExposedDefinition& Channel : GlobalChannels)
	{
		if (Channel.Name.IsNone())
		{
			Channel.Name = "MyChannel";
		}

		while (UsedNames.Contains(Channel.Name))
		{
			Channel.Name.SetNumber(Channel.Name.GetNumber() + 1);
		}

		UsedNames.Add(Channel.Name);

		Channel.Fixup();
	}

	GVoxelChannelManager->UpdateChannelsFromAsset_GameThread(
		this,
		"Project",
		GlobalChannels);
}

#if WITH_EDITOR
void UVoxelSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_OWN_MEMBER_NAME(FramesToAverage))
	{
		FramesToAverage = 1 << FMath::CeilLogTwo(FramesToAverage);
	}

	if (PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive)
	{
		return;
	}

	UpdateChannels();
}
#endif