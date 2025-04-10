
#include "CharacterRisingTips.h"

#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetLayoutLibrary.h>
#include <Components/SizeBox.h>
#include <Components/VerticalBox.h>
#include <Components/TextBlock.h>
#include <Components/Image.h>
#include "Components/WidgetSwitcher.h"

#include "CharacterBase.h"
#include "Planet.h"
#include "GameOptions.h"
#include "TemplateHelper.h"
#include "TextSubSystem.h"
#include "TextCollect.h"

struct FCharacterRisingTips : public TStructVariable<FCharacterRisingTips>
{
	const FName VerticalBox = TEXT("VerticalBox");

	const FName Icon = TEXT("Icon");

	const FName Icon_Disable = TEXT("Icon_Disable");

	const FName Icon_Treatment = TEXT("Icon_Treatment");

	const FName Text = TEXT("Text");

	const FName SizeBox = TEXT("SizeBox");

	const FName WidgetSwitcher = TEXT("WidgetSwitcher");
};

void UCharacterRisingTips::NativeConstruct()
{
	Super::NativeConstruct();

	SetAnchorsInViewport(FAnchors(.5f));
	SetAlignmentInViewport(FVector2D(.5f, .5f));

	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateUObject(this, &ThisClass::ResetPosition)
	);

	PlayMyAnimation(bIsCritical);

	ResetPosition(0.f);
}

void UCharacterRisingTips::NativeDestruct()
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);

	Super::NativeDestruct();
}

void UCharacterRisingTips::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UCharacterRisingTips::PlayAnimationFinished()
{
	RemoveFromParent();
}

bool UCharacterRisingTips::ResetPosition(float InDeltaTime)
{
	FVector2D ScreenPosition = FVector2D::ZeroVector;
	UGameplayStatics::ProjectWorldToScreen(
		UGameplayStatics::GetPlayerController(this, 0),
		TargetCharacterPtr->GetActorLocation(),
		ScreenPosition
	);

	SetPositionInViewport(ScreenPosition);

	return true;
}
