#include "ConversationLayout.h"

#include "GuideActor.h"
#include "GuideInteraction.h"
#include "GuideSystemStateTreeComponent.h"
#include "Kismet/GameplayStatics.h"

#include "HumanCharacter_Player.h"
#include "HumanInteractionWithChallengeEntry.h"
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
			GetWidgetFromName(FConversationLayout::Get().PlayerConversationBorder)
		);
		if (UIPtr)
		{
			UIPtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	// NPC
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
			return;
		}
	}

	// ChallengeEntry
	{
		auto CurrentActionSPtr =
			DynamicCastSharedPtr<HumanProcessor::FHumanInteractionWithChallengeEntryProcessor>(
				UInputProcessorSubSystem::GetInstance()->GetCurrentAction()
			);

		if (CurrentActionSPtr)
		{
			SceneActorInteractionInterfacePtr = CurrentActionSPtr->TargetPtr;

			auto UIPtr = Cast<UOptionList>(GetWidgetFromName(FConversationLayout::Get().InteractionList));
			if (UIPtr)
			{
				UIPtr->SetVisibility(ESlateVisibility::Visible);
				UIPtr->UpdateDisplay(
					CurrentActionSPtr->TargetPtr,
					std::bind(&ThisClass::SelectedInteractionItem, this, std::placeholders::_1)
				);
			}
			return;
		}
	}
}

void UConversationLayout::DisEnable()
{
	GetOptions()->CloseUI();

	if (CharacterPtr)
	{
		CharacterPtr->GetSceneActorInteractionComponent()->StopInteractionItem();
		CharacterPtr = nullptr;
	}
	if (SceneActorInteractionInterfacePtr)
	{
		SceneActorInteractionInterfacePtr->GetSceneActorInteractionComponent()->StopInteractionItem();
		SceneActorInteractionInterfacePtr = nullptr;
	}
}

ELayoutCommon UConversationLayout::GetLayoutType() const
{
	return ELayoutCommon::kConversationLayout;
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

void UConversationLayout::SelectedInteractionItem(
	const TSubclassOf<AGuideInteraction_Actor>& GuideInteractionClass
)
{
	if (CharacterPtr)
	{
		CharacterPtr->GetSceneActorInteractionComponent()->StartInteractionItem(GuideInteractionClass);
	}
	else if (SceneActorInteractionInterfacePtr)
	{
		SceneActorInteractionInterfacePtr->GetSceneActorInteractionComponent()->StartInteractionItem(GuideInteractionClass);
	}

	CloseOption();
}
