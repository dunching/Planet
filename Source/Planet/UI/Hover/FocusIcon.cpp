
#include "FocusIcon.h"

#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetLayoutLibrary.h>
#include <Components/SizeBox.h>

#include "CharacterBase.h"
#include "LogWriter.h"

void UFocusIcon::NativeConstruct()
{
	Super::NativeConstruct();

	SetAnchorsInViewport(FAnchors(.5f));
	SetAlignmentInViewport(FVector2D(.5f, .5f));

	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateUObject(this, &ThisClass::ResetPosition)
	);
	ResetPosition(0.f);
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
		TargetCharacterPtr->GetActorLocation(),
		ScreenPosition,
		true
	);

	SetPositionInViewport(ScreenPosition);

	return true;
}
