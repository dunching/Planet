
#include "TeleportPointWidget.h"

#include "Components/Border.h"
#include "Components/Image.h"

void UTeleportPointWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UTeleportPointWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

FReply UTeleportPointWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
	)
{
 	OnClicked.ExecuteIfBound(this);
 	
	return FReply::Handled();
}

FReply UTeleportPointWidget::NativeOnMouseButtonUp(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
	)
{
	return FReply::Handled();
}
