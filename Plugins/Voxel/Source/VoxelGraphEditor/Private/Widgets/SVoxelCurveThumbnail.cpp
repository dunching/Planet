// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelCurveThumbnail.h"

void SVoxelCurveThumbnail::Construct(const FArguments& InArgs, const FRichCurve* CurveToDisplay)
{
	Size = FVector2D(InArgs._Width, InArgs._Height);
	CurvePoints.Reserve(InArgs._NumPoints);
	CurvePoints.AddZeroed(InArgs._NumPoints);

	UpdateCurve(CurveToDisplay);
}

void SVoxelCurveThumbnail::UpdateCurve(const FRichCurve* CurveToDisplay)
{
	float TimeMin = 0.f;
	float TimeMax = 0.f;

	float ValueMin = 0.f;
	float ValueMax = 0.f;

	if (CurveToDisplay)
	{
		CurveToDisplay->GetTimeRange(TimeMin, TimeMax);
		CurveToDisplay->GetValueRange(ValueMin, ValueMax);
	}

	const float TimeRange = TimeMax - TimeMin;
	const float ValueRange = ValueMax - ValueMin;

	if (ValueRange == 0.f ||
		TimeRange == 0.f)
	{
		for (int32 Index = 0; Index < CurvePoints.Num(); Index++)
		{
			CurvePoints[Index] = FVector2D(Size.X / (CurvePoints.Num() - 1) * Index, Size.Y * 0.5f);
		}

		return;
	}

	const float TimeIncrement = TimeRange / (CurvePoints.Num() - 1);
	for (int32 Index = 0; Index < CurvePoints.Num(); Index++)
	{
		const float Time = TimeMin + Index * TimeIncrement;
		const float Value = CurveToDisplay ? CurveToDisplay->Eval(Time) : 0.f;

		const float NormalizedX = (Time - TimeMin) / TimeRange;
		const float NormalizedY = (Value - ValueMin) / ValueRange;

		CurvePoints[Index] = FVector2D(NormalizedX * Size.X, (1.f - NormalizedY) * Size.Y);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int32 SVoxelCurveThumbnail::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), CurvePoints, ESlateDrawEffect::None, InWidgetStyle.GetForegroundColor(), true, 2.0f);
	return LayerId;
}

FVector2D SVoxelCurveThumbnail::ComputeDesiredSize(float) const
{
	return Size;
}