#include "ConversationLayout.h"

#include "Kismet/GameplayStatics.h"

#include "HumanCharacter_Player.h"
#include "HumanInteractionWithNPC.h"
#include "InputProcessorSubSystem.h"
#include "OptionList.h"
#include "PlayerConversationBorder.h"

namespace HumanProcessor
{
	class FHumanInteractionWithNPCProcessor;
}

struct FConversationLayout : public TStructVariable<FConversationLayout>
{
	FName InteractionList = TEXT("InteractionList");

	FName PlayerConversationBorder = TEXT("PlayerConversationBorder");
};

void UConversationLayout::NativeConstruct()
{
	Super::NativeConstruct();

	Enable();
}

void UConversationLayout::Enable()
{
	{
		auto UIPtr = Cast<UPlayerConversationBorder>(
			GetWidgetFromName(FConversationLayout::Get().PlayerConversationBorder));
		if (UIPtr)
		{
			UIPtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	{
		auto PlayerCharacterPtr = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(this, 0));

		auto CurrentActionSPtr =
			DynamicCastSharedPtr<HumanProcessor::FHumanInteractionWithNPCProcessor>(
				UInputProcessorSubSystem::GetInstance()->GetCurrentAction()
			);

		if (CurrentActionSPtr)
		{
			auto UIPtr = Cast<UOptionList>(GetWidgetFromName(FConversationLayout::Get().InteractionList));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Visible);
				UIPtr->UpdateDisplay(CurrentActionSPtr->CharacterPtr);
			}
		}
	}
}

void UConversationLayout::DisEnable()
{
	auto UIPtr = Cast<UOptionList>(GetWidgetFromName(FConversationLayout::Get().InteractionList));
	if (UIPtr)
	{
		UIPtr->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UConversationLayout::CloseOption()
{
	auto UIPtr = Cast<UOptionList>(GetWidgetFromName(FConversationLayout::Get().InteractionList));
	if (UIPtr)
	{
		UIPtr->SetVisibility(ESlateVisibility::Hidden);
	}
}
