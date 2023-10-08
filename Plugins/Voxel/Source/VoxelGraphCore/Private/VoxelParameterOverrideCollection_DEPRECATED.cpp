// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelParameterOverrideCollection_DEPRECATED.h"
#include "VoxelParameterContainer.h"
#include "VoxelChannelAsset_DEPRECATED.h"

void FVoxelParameterOverrideCollection_DEPRECATED::MigrateTo(UVoxelParameterContainer& ParameterContainer)
{
	// If all parameters are disabled, this is likely an old file
	// Enable all parameters by default
	bool bEnableAll = true;
	for (const FVoxelParameterOverride_DEPRECATED& Parameter : Parameters)
	{
		if (Parameter.bEnable)
		{
			bEnableAll = false;
		}
	}

	for (const FVoxelParameterOverride_DEPRECATED& Parameter : Parameters)
	{
		FVoxelParameterPath Path;
		Path.Guids.Add(Parameter.Parameter.Guid);

		FVoxelParameterValueOverride ValueOverride;
		ValueOverride.bEnable = bEnableAll ? true : Parameter.bEnable;
		ValueOverride.CachedName = Parameter.Parameter.Name;
		ValueOverride.CachedCategory = FName(Parameter.Parameter.Category);
		ValueOverride.Value = Parameter.ValueOverride;

		if (ValueOverride.Value.Is<UVoxelChannelAsset_DEPRECATED>())
		{
			if (UVoxelChannelAsset_DEPRECATED* Channel = ValueOverride.Value.Get<UVoxelChannelAsset_DEPRECATED>())
			{
				ValueOverride.Value = FVoxelPinValue::Make<FVoxelChannelName>(Channel->MakeChannelName());
			}
		}

		ensure(!ParameterContainer.ValueOverrides.Contains(Path));
		ParameterContainer.ValueOverrides.Add(Path, ValueOverride);
	}

	ParameterContainer.Fixup();

	if (bEnableAll)
	{
		// Disable any unneeded override

		for (auto& It : ParameterContainer.ValueOverrides)
		{
			It.Value.bEnable = false;
		}

		ParameterContainer.Fixup();

		for (auto& It : ParameterContainer.ValueOverrides)
		{
			It.Value.bEnable = true;
		}
	}

	Parameters = {};
	Categories = {};
}