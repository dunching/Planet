// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelRuntimeProvider.generated.h"

class FVoxelRuntime;

UINTERFACE()
class UVoxelRuntimeProvider : public UInterface
{
	GENERATED_BODY()
};

class IVoxelRuntimeProvider
{
	GENERATED_BODY()

public:
	virtual TSharedPtr<FVoxelRuntime> GetRuntime() const VOXEL_PURE_VIRTUAL({});
};