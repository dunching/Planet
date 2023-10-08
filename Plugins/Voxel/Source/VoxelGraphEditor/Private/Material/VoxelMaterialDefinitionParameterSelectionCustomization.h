// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"

struct FVoxelMaterialDefinitionToolkit;

class FVoxelMaterialDefinitionParameterSelectionCustomization : public IDetailCustomization
{
public:
	explicit FVoxelMaterialDefinitionParameterSelectionCustomization(const FGuid& TargetParameterId)
		: TargetParameterId(TargetParameterId)
	{
	}

	//~ Begin IDetailCustomization Interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
	//~ End IDetailCustomization Interface

private:
	FGuid TargetParameterId;
	TSharedPtr<FVoxelStructCustomizationWrapper> Wrapper;
};