// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"

class ALandscape;
class UVoxelHeightmap;

struct VOXELLANDMASS_API FVoxelLandscapeConversion
{
	static bool Convert(const ALandscape& Landscape, UVoxelHeightmap& Heightmap);
};