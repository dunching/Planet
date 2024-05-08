// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"

class IVoxelContentEditorModule : public IModuleInterface
{
public:
	int32 Version = 0;

	virtual void ShowContent() = 0;
};