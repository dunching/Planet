#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "ScaleableWidget.generated.h"

class UImage;
class UCanvasPanel;

/**
 * 可被拖住移动、缩放的Widget
 * 类似GTA的地图
 *
 * https://forums.unrealengine.com/t/how-to-scale-zoom-a-widget-with-blueprints/430983/36
 */
UCLASS()
class COMMON_UMG_API UScaleableWidget : public UUserWidget_Override
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
		) override;

	virtual FReply NativeOnMouseButtonUp(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
		) override;

	virtual FReply NativeOnMouseMove(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
		) override;

	virtual FReply NativeOnMouseWheel(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
		) override;

	virtual void NativeOnMouseLeave(
		const FPointerEvent& InMouseEvent
		) override;

protected:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* OuterCanvas = nullptr;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* InnerCanvas = nullptr;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	bool bIsDraggingMap = false;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	int32 CurrentZoomStep = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	float ZoomFactor = .8f;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	float PanSpeed = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	int32 MinZoom = -4;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	int32 MaxZoom = 5;

	FVector2D ClampSides(
		const FGeometry& InGeometry,
		const FVector2D& ClampTo
		) const;
};
