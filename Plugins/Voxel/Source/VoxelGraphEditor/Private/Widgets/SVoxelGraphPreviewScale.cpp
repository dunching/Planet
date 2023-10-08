// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphPreviewScale.h"

void SVoxelGraphPreviewScale::Construct(const FArguments& InArgs)
{
	Value = InArgs._Value;
	Resolution = InArgs._Resolution;

	const auto GetPixelSize = [=]() -> double
	{
		const TSharedPtr<SWidget> Widget = SizeWidget.Pin();
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
	};

	ChildSlot
	[
		SNew(SScaleBox)
		.IgnoreInheritedScale(true)
		[
			SNew(SBox)
			.Visibility(EVisibility::HitTestInvisible)
			.Padding(10.f)
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				.VAlign(VAlign_Bottom)
				[
					SNew(SImage)
					.Image(FVoxelEditorStyle::GetBrush("Graph.Preview.Scale"))
					.DesiredSizeOverride_Lambda([=]
					{
						const double MinSizeCm = GetPixelSize() * 100;
						const int32 Power = FMath::Max(0, FMath::FloorToInt(FMath::LogX(10, MinSizeCm)));
						const double Measurement = MinSizeCm / FVoxelUtilities::IntPow(10, Power);

						double Width;
						if (Measurement > 5)
						{
							Width = 10 / Measurement;
						}
						else if (Measurement > 2)
						{
							Width = 5 / Measurement;
						}
						else if (Measurement > 1)
						{
							Width = 2 / Measurement;
						}
						else
						{
							Width = 1 / Measurement;
						}

						return FVector2D(Width * 100, 10);
					})
				]
				+ SOverlay::Slot()
				.Padding(5.f, 3.f)
				[

					SNew(STextBlock)
					.Font(FVoxelEditorUtilities::Font())
					.Text_Lambda([=]
					{
						const double MinSizeCm = GetPixelSize() * 100;
						const int32 Power = FMath::Max(0, FMath::FloorToInt(FMath::LogX(10, MinSizeCm)));
						const double Measurement = MinSizeCm / FVoxelUtilities::IntPow(10, Power);

						double Width;
						if (Measurement > 5)
						{
							Width = 10;
						}
						else if (Measurement > 2)
						{
							Width = 5;
						}
						else if (Measurement > 1)
						{
							Width = 2;
						}
						else
						{
							Width = 1;
						}

						int32 Size;
						FString MeasurementType;
						if (Power < 0)
						{
							Size = FMath::FloorToInt(Width / FVoxelUtilities::IntPow(10, Power + 1));
							MeasurementType = "mm";
							if (Size == 10)
							{
								Size = 1;
								MeasurementType = "cm";
							}
						}
						else if (Power < 2)
						{
							Size = FMath::FloorToInt(Width * FVoxelUtilities::IntPow(10, Power));
							MeasurementType = "cm";
							if (Size == 100)
							{
								Size = 1;
								MeasurementType = "m";
							}
						}
						else if (Power < 5)
						{
							Size = FMath::FloorToInt(Width * FVoxelUtilities::IntPow(10, Power - 2));
							MeasurementType = "m";
							if (Size == 1000)
							{
								Size = 1;
								MeasurementType = "km";
							}
						}
						else
						{
							Size = FMath::FloorToInt(Width * FVoxelUtilities::IntPow(10, Power - 5));
							MeasurementType = "km";
						}

						return FText::FromString(FString::FromInt(Size) + " " + MeasurementType);
					})
					.ColorAndOpacity(FLinearColor::White)
					.ShadowOffset(FVector2D(1.f))
					.ShadowColorAndOpacity(FLinearColor::Black)
				]
			]
		]
	];
}