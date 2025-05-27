#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScaleableWidget.generated.h"

class UImage;
class UCanvasPanel;

/**
 * 可被拖住移动、缩放的Widget
 * 类似GTA的地图
 */
UCLASS()
class COMMON_UMG_API UScaleableWidget : public UUserWidget
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

private:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* OuterCanvas = nullptr;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* InnerCanvas = nullptr;

	UPROPERTY(EditAnywhere, Category = "Map")
	bool bIsDraggingMap = false;

	UPROPERTY(EditAnywhere, Category = "Map")
	int32 CurrentZoomStep = 0;

	UPROPERTY(EditAnywhere, Category = "Map")
	float ZoomFactor = .8f;

	UPROPERTY(EditAnywhere, Category = "Map")
	float PanSpeed = 1.f;

	UPROPERTY(EditAnywhere, Category = "Map")
	int32 MinZoom = -4;

	UPROPERTY(EditAnywhere, Category = "Map")
	int32 MaxZoom = 5;

	FVector2D ClampSides(
		const FGeometry& InGeometry,
		const FVector2D& ClampTo
		) const;
};
