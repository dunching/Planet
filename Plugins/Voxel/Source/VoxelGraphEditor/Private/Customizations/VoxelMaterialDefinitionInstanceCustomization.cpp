// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "Material/VoxelMaterialDefinitionInstance.h"
#include "Customizations/VoxelParameterContainerDetails.h"

VOXEL_CUSTOMIZE_CLASS(UVoxelMaterialDefinitionInstance)(IDetailLayoutBuilder& DetailLayout)
{
	const TSharedRef<IPropertyHandle> ParameterContainerHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UVoxelMaterialDefinitionInstance, ParameterContainer));
	KeepAlive(FVoxelParameterContainerDetails::Create(DetailLayout, ParameterContainerHandle));
}