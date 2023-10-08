// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"

class SVoxelCurveThumbnail : public SLeafWidget
{
public:
	VOXEL_SLATE_ARGS()
	{
		FArguments()
			: _NumPoints(13)
			, _Width(16.f)
			, _Height(8.f)
		{
		}

		SLATE_ARGUMENT(int32, NumPoints)
		SLATE_ARGUMENT(float, Width)
		SLATE_ARGUMENT(float, Height)
	};

	void Construct(const FArguments& InArgs, const FRichCurve* CurveToDisplay);

	void UpdateCurve(const FRichCurve* CurveToDisplay);

protected:
	//~ Begin SLeafWidget Interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	//~ End SLeafWidget Interface

private:
	TArray<FVector2D> CurvePoints;
	FVector2D Size;
};