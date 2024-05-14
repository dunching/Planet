
#include "FocusIcon.h"

#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetLayoutLibrary.h>
#include <Components/SizeBox.h>


void UFocusIcon::NativeConstruct()
{
	Super::NativeConstruct();

	SetAnchorsInViewport(FAnchors(.5f));

	auto SizeBoxPtr = Cast<USizeBox>(GetWidgetFromName(TEXT("SizeBox")));
	if (SizeBoxPtr)
	{
		SizeBox.X = SizeBoxPtr->GetWidthOverride();
		SizeBox.Y = SizeBoxPtr->GetHeightOverride();
	}

	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateUObject(this, &ThisClass::ResetPosition)
	);
}

void UFocusIcon::NativeDestruct()
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);

	Super::NativeDestruct();
}

void UFocusIcon::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);


}

bool UFocusIcon::ResetPosition(float InDeltaTime)
{
	FVector2D ScreenPosition = FVector2D::ZeroVector;
	UGameplayStatics::ProjectWorldToScreen(
		UGameplayStatics::GetPlayerController(this, 0),
		FocusItem.Actor->GetActorLocation(),
		ScreenPosition
	);

	const auto Scale = UWidgetLayoutLibrary::GetViewportScale(this);

	const auto TempWidgetSize = SizeBox * Scale;

	ScreenPosition -= TempWidgetSize / 2;

	SetPositionInViewport(ScreenPosition);

	return true;
}
