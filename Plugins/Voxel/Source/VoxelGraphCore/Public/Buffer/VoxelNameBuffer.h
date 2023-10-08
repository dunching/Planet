// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelBuffer.h"
#include "VoxelNameBuffer.generated.h"

DECLARE_VOXEL_TERMINAL_BUFFER(FVoxelNameBuffer, FVoxelNameWrapper);

USTRUCT(DisplayName = "Name Buffer")
struct VOXELGRAPHCORE_API FVoxelNameBuffer final : public FVoxelSimpleTerminalBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_TERMINAL_BUFFER_BODY(FVoxelNameBuffer, FVoxelNameWrapper);
};