
#include "InteractionToAIMenu.h"

#include "Components/HorizontalBox.h"
#include <Kismet/GameplayStatics.h>

#include "InteractionBtn.h"
#include "CharacterBase.h"
#include "Pawn/PawnIteractionComponent.h"
#include <AssetRefrencePath.h>
#include "AssetRefMap.h"

void UInteractionToAIMenu::InteractionAICharacter()
{
}

void UInteractionToAIMenu::OnClickViewBackpack()
{
	auto HorizonBoxPtr = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("HorizontalBox")));
	if (HorizonBoxPtr)
	{
		HorizonBoxPtr->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInteractionToAIMenu::OnClickUnion()
{
	if (TargetCharacterPtr)
	{
	}
}

void UInteractionToAIMenu::OnClickTheft()
{
}
