// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelPointHandle.h"
#include "VoxelPointHandleProvider.generated.h"

UINTERFACE()
class UVoxelPointHandleProvider : public UInterface
{
	GENERATED_BODY()
};

class IVoxelPointHandleProvider
{
	GENERATED_BODY()

public:
	virtual bool TryGetPointHandle(
		int32 ItemIndex,
		FVoxelPointHandle& OutHandle) const VOXEL_PURE_VIRTUAL({});
};