// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMacroLibraryToolkit.h"
#include "VoxelGraphToolkit.h"

TArray<FVoxelToolkit::FMode> FVoxelMacroLibraryToolkit::GetModes() const
{
	FMode Mode;
	Mode.Struct = FVoxelGraphToolkit::StaticStruct();
	Mode.Object = Asset->Graph;
	Mode.ConfigureToolkit = [](FVoxelToolkit& Toolkit)
	{
		const TSharedRef<FVoxelGraphToolkit> GraphToolkit = StaticCastSharedRef<FVoxelGraphToolkit>(Toolkit.AsShared());
		GraphToolkit->bIsMacroLibrary = true;
	};
	return { Mode };
}