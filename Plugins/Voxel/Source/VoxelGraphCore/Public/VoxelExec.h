// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelExec.generated.h"

class IVoxelExecNodeRuntimeInterface;

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelExec
{
	GENERATED_BODY()

	TFunction<TSharedPtr<IVoxelExecNodeRuntimeInterface>()> MakeRuntime;
};