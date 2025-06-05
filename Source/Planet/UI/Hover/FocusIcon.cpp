#include "FocusIcon.h"

#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetLayoutLibrary.h>
#include <Components/SizeBox.h>

#include "CharacterAbilitySystemComponent.h"
#include "CharacterBase.h"
#include "LogWriter.h"

void UFocusIcon::NativeConstruct()
{
	Super::NativeConstruct();

	SetAnchorsInViewport(FAnchors(.5f));
	SetAlignmentInViewport(FVector2D(.5f, .5f));
}

void UFocusIcon::NativeDestruct()
{
	Super::NativeDestruct();
}

void UFocusIcon::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime
	)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FVector UFocusIcon::GetHoverPosition()
{
	if (TargetCharacterPtr.IsValid())
	{
		return TargetCharacterPtr->GetActorLocation();
	}

	return Super::GetHoverPosition();
}
