// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphPreviewImage.h"

VOXEL_INITIALIZE_STYLE(GraphPreviewImage)
{
	Set("Graph.Preview.PositionMarker", new IMAGE_BRUSH("Graphs/Preview_PositionMarker", CoreStyleConstants::Icon16x16));
}

void SVoxelGraphPreviewImage::Construct(const FArguments& Args)
{
	PositionMarkerHalfSize = Args._PositionMarkerSize / 2.f;

	ChildSlot
	[
		SAssignNew(Box, SBox)
		.MinDesiredWidth(Args._Width)
		.MinDesiredHeight(Args._Height)
		.Content()
		[
			Args._Content.Widget
		]
	];
}

void SVoxelGraphPreviewImage::SetLockedPosition(const FVector& LockedPosition_World)
{
	bHasLockedPosition = true;
	Position_World = LockedPosition_World;
}

bool SVoxelGraphPreviewImage::UpdateLockedPosition(const FMatrix& PixelToWorld, const FVector2D& Offset)
{
	FVector2D PixelPosition = FVector2D(PixelToWorld.InverseTransformPosition(Position_World));
	PixelPosition.Y = Offset.Y - PixelPosition.Y;
	LocalPosition = PixelPosition;

	if (LocalPosition.X >= PositionMarkerHalfSize &&
		LocalPosition.X <= Offset.X - PositionMarkerHalfSize &&
		LocalPosition.Y >= PositionMarkerHalfSize &&
        LocalPosition.Y <= Offset.Y - PositionMarkerHalfSize)
	{
		return true;
	}

	bHasLockedPosition = false;
	return false;
}

void SVoxelGraphPreviewImage::ClearLockedPosition()
{
	bHasLockedPosition = false;
}

int32 SVoxelGraphPreviewImage::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	LayerId = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	if (!bHasLockedPosition)
	{
		return LayerId;
	}
	LayerId++;

	const float InverseScale = Inverse(AllottedGeometry.Scale);

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(TransformVector(InverseScale, FVector2D(PositionMarkerHalfSize * 2.f)), FSlateLayoutTransform(LocalPosition - PositionMarkerHalfSize * InverseScale)),
		FVoxelEditorStyle::GetBrush("Graph.Preview.PositionMarker"));

	return LayerId;
}