// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "VoxelGraphInstance.h"
#include "Customizations/VoxelParameterContainerDetails.h"

VOXEL_CUSTOMIZE_CLASS(UVoxelGraphInstance)(IDetailLayoutBuilder& DetailLayout)
{
	const TSharedRef<IPropertyHandle> ParameterContainerHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UVoxelGraphInstance, ParameterContainer));
	KeepAlive(FVoxelParameterContainerDetails::Create(DetailLayout, ParameterContainerHandle));
}