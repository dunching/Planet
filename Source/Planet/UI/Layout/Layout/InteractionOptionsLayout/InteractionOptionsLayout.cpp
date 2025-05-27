#include "InteractionOptionsLayout.h"

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
#include "Components/Button.h"

class AMainHUD;

namespace HumanProcessor
{
	class FHumanRegularProcessor;
	class FHumanInteractionWithNPCProcessor;
}

struct FInteractionOptionsLayout : public TStructVariable<FInteractionOptionsLayout>
{
	FName InteractionList = TEXT("InteractionList");

	FName PlayerConversationBorder = TEXT("PlayerConversationBorder");

	FName QuitBtn = TEXT("QuitBtn");
};

void UInteractionOptionsLayout::NativeConstruct()
{
	Super::NativeConstruct();

	{
		auto UIPtr = Cast<UButton>(
			GetWidgetFromName(FInteractionOptionsLayout::Get().QuitBtn)
		);
		if (UIPtr)
		{
			UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnQuitBtnClicked);
		}
	}
}

void UInteractionOptionsLayout::Enable()
{
	{
		auto UIPtr = Cast<UPlayerConversationBorder>(
			GetWidgetFromName(FInteractionOptionsLayout::Get().PlayerConversationBorder)
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
			SceneActorInteractionInterfacePtr = CurrentActionSPtr->CharacterPtr;

			auto UIPtr = Cast<UOptionList>(GetWidgetFromName(FInteractionOptionsLayout::Get().InteractionList));
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

			auto UIPtr = Cast<UOptionList>(GetWidgetFromName(FInteractionOptionsLayout::Get().InteractionList));
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

void UInteractionOptionsLayout::DisEnable()
{
	GetOptions()->CloseUI();

	// if (CharacterPtr)
	// {
	// 	CharacterPtr->GetSceneActorInteractionComponent()->StopInteractionItem();
	// 	CharacterPtr = nullptr;
	// }
	// if (SceneActorInteractionInterfacePtr)
	// {
	// 	SceneActorInteractionInterfacePtr->GetSceneActorInteractionComponent()->StopInteractionItem();
	// 	SceneActorInteractionInterfacePtr = nullptr;
	// }
}

ELayoutCommon UInteractionOptionsLayout::GetLayoutType() const
{
	return ELayoutCommon::kOptionLayout;
}

UOptionList* UInteractionOptionsLayout::GetOptions() const
{
	auto UIPtr = Cast<UOptionList>(GetWidgetFromName(FInteractionOptionsLayout::Get().InteractionList));
	if (UIPtr)
	{
		return UIPtr;
	}

	return nullptr;
}

void UInteractionOptionsLayout::CloseOption()
{
	auto UIPtr = Cast<UOptionList>(GetWidgetFromName(FInteractionOptionsLayout::Get().InteractionList));
	if (UIPtr)
	{
		UIPtr->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInteractionOptionsLayout::SelectedInteractionItem(
	const TSubclassOf<AGuideInteraction_Actor>& GuideInteractionClass
)
{
	if (SceneActorInteractionInterfacePtr)
	{
		SceneActorInteractionInterfacePtr->GetSceneActorInteractionComponent()->StartInteractionItem(GuideInteractionClass);
	}

	CloseOption();
}

void UInteractionOptionsLayout::OnQuitBtnClicked()
{
	UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
}
