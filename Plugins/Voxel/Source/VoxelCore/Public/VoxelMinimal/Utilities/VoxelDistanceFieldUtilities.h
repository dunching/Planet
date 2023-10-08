// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"

namespace FVoxelUtilities
{
	VOXELCORE_API void JumpFlood2D(const FIntPoint& Size, TArrayView<FIntPoint> InOutClosestPosition);
}