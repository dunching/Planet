// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "Nodes/VoxelGraphMacroNode.h"

VOXEL_CUSTOMIZE_CLASS(UVoxelGraphMacroNode)(IDetailLayoutBuilder& DetailLayout)
{
	DetailLayout.HideCategory("Preview");
}