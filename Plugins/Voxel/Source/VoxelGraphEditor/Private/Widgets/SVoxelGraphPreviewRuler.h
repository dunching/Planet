// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"

class SVoxelGraphPreviewRuler : public SCompoundWidget
{
public:
	VOXEL_SLATE_ARGS()
	{
		SLATE_ATTRIBUTE(double, Value)
		SLATE_ATTRIBUTE(int32, Resolution)
		SLATE_ATTRIBUTE(TWeakPtr<SWidget>, SizeWidget)
	};

	void Construct(const FArguments& InArgs);

public:
	void StartRuler(const FVector2D& ScreenSpacePosition, const FVector2D& TransformedPosition);
	void StopRuler() const;

	void UpdateRuler(const FVector2D& ScreenSpacePosition, const FVector2D& TransformedPosition);

private:
	double GetPixelSize() const;
	double GetRulerDistance() const;
	FString GetDistanceText() const;

private:
	TAttribute<double> Value;
	TAttribute<int32> Resolution;
	TAttribute<TWeakPtr<SWidget>> SizeWidget;

	FVector2D StartPosition;
	FVector2D EndPosition;

	FVector2D TransformedStartPosition;
	FVector2D TransformedEndPosition;

	FString DistanceText;

	TSharedPtr<SWidget> Canvas;
	TSharedPtr<SWidget> ImageBox;
	TSharedPtr<SWidget> TextBox;
};