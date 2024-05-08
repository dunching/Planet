// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelBuffer.h"
#include "VoxelClassBuffer.generated.h"

DECLARE_VOXEL_TERMINAL_BUFFER(FVoxelClassBuffer, TSubclassOf<UObject>);

USTRUCT(DisplayName = "Class Buffer")
struct VOXELGRAPHCORE_API FVoxelClassBuffer final : public FVoxelSimpleTerminalBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_TERMINAL_BUFFER_BODY(FVoxelClassBuffer, TSubclassOf<UObject>);
};