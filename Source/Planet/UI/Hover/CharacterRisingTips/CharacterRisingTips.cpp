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
#include "OnEffectedTawrgetCallback.h"
#include "PlanetGameViewportClient.h"
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
}

void UCharacterRisingTips::SetData(
	const FOnEffectedTawrgetCallback& ReceivedEventModifyDataCallback
)
{
	if (!ReceivedEventModifyDataCallback.TargetCharacterPtr)
	{
		return;
	}

	TargetCharacterPtr = ReceivedEventModifyDataCallback.TargetCharacterPtr;

	PlayMyAnimation(bIsCritical);
}

FVector UCharacterRisingTips::GetHoverPosition()
{
	return TargetCharacterPtr->GetActorLocation();
}

void UCharacterRisingTips::PlayAnimationFinished()
{
	auto ScreenLayer = UKismetGameLayerManagerLibrary::GetGameLayer<FHoverWidgetScreenLayer>(
		GetWorld(),
		TargetPointSharedLayerName
	);
	if (ScreenLayer)
	{
		ScreenLayer->RemoveHoverWidget(this);
	}
}