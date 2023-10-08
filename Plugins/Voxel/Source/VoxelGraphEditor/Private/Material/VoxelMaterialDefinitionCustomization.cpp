// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "Material/VoxelMaterialDefinition.h"

VOXEL_CUSTOMIZE_CLASS(UVoxelMaterialDefinition)(IDetailLayoutBuilder& DetailLayout)
{
	DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UVoxelMaterialDefinition, Parameters))->MarkHiddenByCustomization();
	DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UVoxelMaterialDefinition, GuidToParameterData))->MarkHiddenByCustomization();
}