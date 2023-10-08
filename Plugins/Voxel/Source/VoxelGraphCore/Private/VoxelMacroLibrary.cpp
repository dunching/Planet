// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMacroLibrary.h"

DEFINE_VOXEL_FACTORY(UVoxelMacroLibrary);

UVoxelMacroLibrary::UVoxelMacroLibrary()
{
	Graph = CreateDefaultSubobject<UVoxelGraph>("Graph");
}