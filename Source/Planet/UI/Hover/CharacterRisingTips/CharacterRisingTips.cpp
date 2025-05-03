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
#include "Components/Button.h"
#include "Kismet/KismetStringLibrary.h"

struct FCharacterRisingTips : public TStructVariable<FCharacterRisingTips>
{
	const FName VerticalBox = TEXT("VerticalBox");

	const FName Icon_Treatment = TEXT("Icon_Treatment");

	const FName Text = TEXT("Text");
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

	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FCharacterRisingTips::Get().Text));
	if (UIPtr)
	{
		UIPtr->SetText(
		               FText::FromString(
		                                 UKismetStringLibrary::Conv_IntToString(
		                                                                        ReceivedEventModifyDataCallback.
		                                                                        TherapeuticalDose > 0 ?
			                                                                        ReceivedEventModifyDataCallback.
			                                                                        TherapeuticalDose :
			                                                                        ReceivedEventModifyDataCallback.
			                                                                        Damage
		                                                                       )
		                                )
		              );
	}

	PlayMyAnimation(bIsCritical, ReceivedEventModifyDataCallback.TherapeuticalDose > 0);
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
