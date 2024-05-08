// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"

class SVoxelGraphPreviewScale : public SCompoundWidget
{
public:
	TWeakPtr<SWidget> SizeWidget;

	VOXEL_SLATE_ARGS()
	{
		SLATE_ATTRIBUTE(double, Value)
		SLATE_ATTRIBUTE(int32, Resolution)
	};

	void Construct(const FArguments& InArgs);

private:
	TAttribute<double> Value;
	TAttribute<int32> Resolution;
};