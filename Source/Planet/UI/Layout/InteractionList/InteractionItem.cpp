#include "InteractionItem.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"


#include "GuideActor.h"
#include "InputProcessorSubSystem.h"
#include "InteractionList.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "PlanetPlayerController.h"
#include "HumanRegularProcessor.h"
#include "HumanCharacter_Player.h"
#include "TextCollect.h"
#include "TextSubSystem.h"

namespace HumanProcessor
{
	class FHumanRegularProcessor;
}

class AMainHUD;

struct FInteractionItem : public TStructVariable<FInteractionItem>
{
	const FName Text = TEXT("Text");

	const FName Btn = TEXT("Btn");
};

void UInteractionItem::NativeConstruct()
{
	Super::NativeConstruct();

	auto UIPtr = Cast<UButton>(GetWidgetFromName(FInteractionItem::Get().Btn));
	if (!UIPtr)
	{
		return;
	}

	UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnClicked);
}

void UInteractionItem::SetData(const FOnInteractionItemClicked & InOnInteractionItemClicked)
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FInteractionItem::Get().Text));
	if (!UIPtr)
	{
		return;
	}

	UIPtr->SetText(FText::FromString(UTextSubSystem::GetInstance()->GetText(TextCollect::CharacterInteraction)));
}

void UInteractionItem::Enable()
{
	ILayoutItemInterfacetion::Enable();
}

void UInteractionItem::DisEnable()
{
	ILayoutItemInterfacetion::DisEnable();
}

void UInteractionItem::OnClicked()
{
	OnInteractionItemClicked.Broadcast();
}
