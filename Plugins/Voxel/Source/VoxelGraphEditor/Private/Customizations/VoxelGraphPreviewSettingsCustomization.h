// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"

struct FVoxelGraphToolkit;

class FVoxelGraphPreviewSettingsCustomization : public IDetailCustomization
{
public:
	FVoxelGraphPreviewSettingsCustomization() = default;

	//~ Begin IDetailCustomization Interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
	//~ End IDetailCustomization Interface

private:
	TSharedPtr<FVoxelStructCustomizationWrapper> Wrapper;
};