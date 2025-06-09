#include "ScaleableWidget.h"

#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"

void UScaleableWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (OuterCanvas)
	{
		OuterCanvas->SetClipping(EWidgetClipping::ClipToBounds);
	}
}

FReply UScaleableWidget::NativeOnMouseButtonUp(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
	)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsDraggingMap = false;

		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UScaleableWidget::NativeOnMouseMove(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
	)
{
	if (bIsDraggingMap)
	{
		const auto InnerCanvasRenderTransform = InnerCanvas->GetRenderTransform();

		const auto NewTranslation = ClampSides(
		                                       InGeometry,
		                                       InnerCanvasRenderTransform.Translation + (
			                                       PanSpeed * InMouseEvent.GetCursorDelta())
		                                      );

		InnerCanvas->SetRenderTranslation(NewTranslation);
	}

	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UScaleableWidget::NativeOnMouseWheel(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
	)
{
	const auto WheelDelta = InMouseEvent.GetWheelDelta();

	auto Lambda = [this, &InGeometry, &InMouseEvent, WheelDelta]
	{
		const auto CurZoomFactor = WheelDelta > 0 ? 1 / ZoomFactor : ZoomFactor;

		if (WheelDelta < 0)
		{
			const auto InnerCanvas_DesiredSize = InnerCanvas->GetDesiredSize();
			const auto OuterCanvas_DesiredSize = InGeometry.GetLocalSize();

			const auto NewScale = InnerCanvas->GetRenderTransform().Scale * (CurZoomFactor);

			const auto NewSize = InnerCanvas_DesiredSize * NewScale;
			if (NewSize.X < OuterCanvas_DesiredSize.X)
			{
				InnerCanvas->SetRenderScale(FVector2D(OuterCanvas_DesiredSize.X / InnerCanvas_DesiredSize.X));
			}
			else if (NewSize.Y < OuterCanvas_DesiredSize.Y)
			{
				InnerCanvas->SetRenderScale(FVector2D(OuterCanvas_DesiredSize.Y / InnerCanvas_DesiredSize.Y));
			}
			else
			{
				InnerCanvas->SetRenderScale(NewScale);
			}
		}
		else
		{
			const auto NewScale = InnerCanvas->GetRenderTransform().Scale * (CurZoomFactor);

			InnerCanvas->SetRenderScale(NewScale);
		}

		const auto InnerCanvasTranslation = InnerCanvas->GetRenderTransform().Translation;

		const auto OffsetTranslation1 = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition()) - (
			                                InGeometry.GetLocalSize() / 2);

		const auto OffsetTranslation2 = OffsetTranslation1 - InnerCanvasTranslation;

		const auto OffsetTranslation3 = InnerCanvasTranslation - (OffsetTranslation2 * (CurZoomFactor - 1));

		InnerCanvas->SetRenderTranslation(
		                                  ClampSides(InGeometry, OffsetTranslation3)
		                                 );
	};

	if (WheelDelta > 0)
	{
		if (CurrentZoomStep < MaxZoom)
		{
			CurrentZoomStep++;

			Lambda();

			return FReply::Handled();
		}
	}
	else
	{
		if (CurrentZoomStep > MinZoom)
		{
			CurrentZoomStep--;

			Lambda();

			return FReply::Handled();
		}
	}

	return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
}

void UScaleableWidget::NativeOnMouseLeave(
	const FPointerEvent& InMouseEvent
	)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	bIsDraggingMap = false;
}


FVector2D UScaleableWidget::ClampSides(
	const FGeometry& InGeometry,
	const FVector2D& ClampTo
	) const
{
	const auto LocalSize = InGeometry.GetLocalSize();

	const auto DesiredSize = InnerCanvas->GetDesiredSize();
	const auto InnerCanvasRenderTransform = InnerCanvas->GetRenderTransform();

	const auto Scale = DesiredSize * InnerCanvasRenderTransform.Scale;

	const auto Value = (Scale - LocalSize) * .5f;

	const auto X = FMath::Clamp(ClampTo.X, Value.X * -1.f, Value.X);
	const auto Y = FMath::Clamp(ClampTo.Y, Value.Y * -1.f, Value.Y);

	return FVector2D(X, Y);
}

FReply UScaleableWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
	)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsDraggingMap = true;

		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
