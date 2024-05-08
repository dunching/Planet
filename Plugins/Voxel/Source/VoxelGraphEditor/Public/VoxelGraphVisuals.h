// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelPinType.h"

class SGraphPin;

struct VOXELGRAPHEDITOR_API FVoxelGraphVisuals
{
	static FSlateIcon GetNodeIcon(const FString& IconName);
	static FLinearColor GetNodeColor(const FString& ColorName);

	static FSlateIcon GetPinIcon(const FVoxelPinType& Type);
	static FLinearColor GetPinColor(const FVoxelPinType& Type);
	static TSharedPtr<SGraphPin> GetPinWidget(UEdGraphPin* Pin);
};