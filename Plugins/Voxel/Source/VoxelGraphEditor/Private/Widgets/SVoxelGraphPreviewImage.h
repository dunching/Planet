// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"

class SVoxelGraphPreviewImage : public SCompoundWidget
{
public:
	VOXEL_SLATE_ARGS()
	{
		FArguments() : _PositionMarkerSize(16.f)
		{
		}

		SLATE_ATTRIBUTE(FOptionalSize, Width);
		SLATE_ATTRIBUTE(FOptionalSize, Height);
		SLATE_ARGUMENT(float, PositionMarkerSize);
		SLATE_DEFAULT_SLOT(FArguments, Content)
	};

	void Construct(const FArguments& Args);

	void SetLockedPosition(const FVector& LockedPosition_World);
	bool UpdateLockedPosition(const FMatrix& PixelToWorld, const FVector2D& Offset);
	void ClearLockedPosition();

	//~ Begin SCompoundWidget Interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	//~ End SCompoundWidget Interface

private:
	TSharedPtr<SBox> Box;

	float PositionMarkerHalfSize = 0.f;

	bool bHasLockedPosition = false;
	FVector Position_World = FVector::ZeroVector;
	FVector2D LocalPosition = FVector2D::ZeroVector;
};