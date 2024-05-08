// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphPreviewRuler.h"
#include "Fonts/FontMeasure.h"

void SVoxelGraphPreviewRuler::Construct(const FArguments& InArgs)
{
	Value = InArgs._Value;
	Resolution = InArgs._Resolution;
	SizeWidget = InArgs._SizeWidget;

	ChildSlot
	[
		SNew(SScaleBox)
		.IgnoreInheritedScale(true)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SAssignNew(Canvas, SCanvas)
			.Visibility(EVisibility::HitTestInvisible)
			+ SCanvas::Slot()
			.Position_Lambda([this] { return StartPosition; })
			.Size_Lambda([this] { return FVector2D(FVector2D::Distance(EndPosition, StartPosition), 8.f); })
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SAssignNew(ImageBox, SBox)
				.Visibility(EVisibility::Collapsed)
				.Padding(FMargin(0.f, -8.f, 0.f, 0.f))
				.RenderTransform_Lambda([this]
				{
					const FVector2D UnitVector = (EndPosition - StartPosition).GetSafeNormal();
					return FSlateRenderTransform(FQuat2D(FMath::Atan2(UnitVector.Y, UnitVector.X)));
				})
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.WidthOverride(FVector2D::Distance(EndPosition, StartPosition))
				.HeightOverride(8.f)
				[
					SNew(SImage)
					.Image(FVoxelEditorStyle::GetBrush("Graph.Preview.Ruler"))
					.ColorAndOpacity(FLinearColor::White)
				]
			]
			+ SCanvas::Slot()
			.Position_Lambda([this]
			{
				const float TextWidth = FSlateApplication::Get().GetRenderer()->GetFontMeasureService()->Measure(DistanceText, FAppStyle::GetFontStyle("PropertyWindow.FilterFont")).X / 2.f;
				const FVector2D UnitVector = (EndPosition - StartPosition).GetSafeNormal();
				const FVector2D TextSize = FVector2D(TextWidth, ((FMath::Cos(UnitVector.Y) - 0.5f) * 2.f) * 20.f);

				return (StartPosition - TextSize) + (UnitVector * FVector2D::Distance(EndPosition, StartPosition) / 2.f);
			})
			.Size_Lambda([this]
			{
				return FSlateApplication::Get().GetRenderer()->GetFontMeasureService()->Measure(DistanceText, FAppStyle::GetFontStyle("PropertyWindow.FilterFont")) + 3.f;
			})
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SAssignNew(TextBox, STextBlock)
				.Visibility(EVisibility::Collapsed)
				.Font(FAppStyle::GetFontStyle("PropertyWindow.FilterFont"))
				.Text_Lambda([this]
				{
					return FText::FromString(DistanceText);
				})
				.ColorAndOpacity(FLinearColor::White)
				.ShadowOffset(FVector2D(1.f))
				.ShadowColorAndOpacity(FLinearColor::Black)
			]
		]
	];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelGraphPreviewRuler::StartRuler(const FVector2D& ScreenSpacePosition, const FVector2D& TransformedPosition)
{
	ImageBox->SetVisibility(EVisibility::HitTestInvisible);
	TextBox->SetVisibility(EVisibility::HitTestInvisible);
	StartPosition = Canvas->GetCachedGeometry().AbsoluteToLocal(ScreenSpacePosition);

	EndPosition = StartPosition;

	TransformedStartPosition = TransformedPosition;
	TransformedEndPosition = TransformedPosition;

	DistanceText = GetDistanceText();
}

void SVoxelGraphPreviewRuler::StopRuler() const
{
	ImageBox->SetVisibility(EVisibility::Collapsed);
	TextBox->SetVisibility(EVisibility::Collapsed);
}

void SVoxelGraphPreviewRuler::UpdateRuler(const FVector2D& ScreenSpacePosition, const FVector2D& TransformedPosition)
{
	EndPosition = Canvas->GetCachedGeometry().AbsoluteToLocal(ScreenSpacePosition);
	TransformedEndPosition = TransformedPosition;

	DistanceText = GetDistanceText();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

double SVoxelGraphPreviewRuler::GetPixelSize() const
{
	const TSharedPtr<SWidget> Widget = SizeWidget.Get().Pin();
	if (!ensure(Widget.IsValid()))
	{
		return 0;
	}

	const double PixelSize = Value.Get() * double(Resolution.Get()) / FMath::Min(Widget->GetCachedGeometry().Size.X, Widget->GetCachedGeometry().Size.Y);

	if (!FMath::IsFinite(PixelSize) ||
		PixelSize <= 0)
	{
		return 1;
	}

	return PixelSize;
}

double SVoxelGraphPreviewRuler::GetRulerDistance() const
{
	const double RawDistance = FVector2D::Distance(TransformedEndPosition, TransformedStartPosition);

	const TSharedPtr<SWidget> Widget = SizeWidget.Get().Pin();
	if (!ensure(Widget.IsValid()))
	{
		return RawDistance;
	}

	return RawDistance / (double(Resolution.Get()) / FMath::Min(Widget->GetCachedGeometry().Size.X, Widget->GetCachedGeometry().Size.Y));
}

FString SVoxelGraphPreviewRuler::GetDistanceText() const
{
	FNumberFormattingOptions Options;
	Options.MinimumFractionalDigits = 2.f;
	Options.MaximumFractionalDigits = 2.f;

	float Size = GetPixelSize() * GetRulerDistance();

	FString MeasurementType;
	if (Size < 1.f)
	{
		Size = (Size * 10.f);
		MeasurementType = "mm";
	}
	else if (Size < 100.f)
	{
		MeasurementType = "cm";
	}
	else if (Size < 100000.f)
	{
		Size = Size / 100.f;
		MeasurementType = "m";
	}
	else
	{
		Size = Size / 100000.f;
		MeasurementType = "km";
	}

	return FText::AsNumber(Size, &Options).ToString() + " " + MeasurementType;
}