// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "KismetPins/SGraphPinObject.h"

class SVoxelGraphPinObject : public SGraphPinObject
{
public:
	VOXEL_SLATE_ARGS()
	{
	};

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

	//~ Begin SGraphPinObject Interface
	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;
	virtual void OnAssetSelectedFromPicker(const FAssetData& AssetData) override;
	//~ End SGraphPinObject Interface
};