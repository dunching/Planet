#include "ConversationLayout.h"

#include "GuideActor.h"
#include "GuideInteraction.h"
#include "GuideSystemStateTreeComponent.h"
#include "Kismet/GameplayStatics.h"

#include "HumanCharacter_Player.h"
#include "HumanInteractionWithNPC.h"
#include "InputProcessorSubSystem.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "OptionList.h"
#include "PlanetPlayerController.h"
#include "PlayerConversationBorder.h"
#include "HumanRegularProcessor.h"

class AMainHUD;

namespace HumanProcessor
{
	class FHumanRegularProcessor;
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
		auto CurrentActionSPtr =
			DynamicCastSharedPtr<HumanProcessor::FHumanInteractionWithNPCProcessor>(
				UInputProcessorSubSystem::GetInstance()->GetCurrentAction()
			);

		if (CurrentActionSPtr)
		{
			CharacterPtr = CurrentActionSPtr->CharacterPtr;
			
			auto UIPtr = Cast<UOptionList>(GetWidgetFromName(FConversationLayout::Get().InteractionList));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Visible);
				UIPtr->UpdateDisplay(
					CurrentActionSPtr->CharacterPtr,
					std::bind(&ThisClass::SelectedInteractionItem, this, std::placeholders::_1)
				);
			}
		}
	}
}

void UConversationLayout::DisEnable()
{
	GetOptions()->CloseUI();

	if (CharacterPtr)
	{
		CharacterPtr->GetSceneActorInteractionComponent()->StopInteractionItem();
	}
}

UOptionList* UConversationLayout::GetOptions() const
{
	auto UIPtr = Cast<UOptionList>(GetWidgetFromName(FConversationLayout::Get().InteractionList));
	if (UIPtr)
	{
		return UIPtr;
	}

	return nullptr;
}

void UConversationLayout::CloseOption()
{
	auto UIPtr = Cast<UOptionList>(GetWidgetFromName(FConversationLayout::Get().InteractionList));
	if (UIPtr)
	{
		UIPtr->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UConversationLayout::SelectedInteractionItem(const TSubclassOf<AGuideInteraction_Actor>& GuideInteractionClass)
{
	if (CharacterPtr)
	{
		CharacterPtr->GetSceneActorInteractionComponent()->StartInteractionItem(GuideInteractionClass);
	}

	CloseOption();
}
