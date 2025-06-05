#include "InteractionOptionsLayout.h"

#include "Components/Button.h"

#include "GuideInteraction.h"
#include "HumanInteractionWithChallengeEntry.h"
#include "HumanInteractionWithNPC.h"
#include "InputProcessorSubSystemBase.h"
#include "MainHUD.h"
#include "OptionList.h"
#include "ChallengeEntry.h"
#include "PlayerConversationBorder.h"
#include "HumanRegularProcessor.h"
#include "HumanCharacter_Player.h"
#include "InputProcessorSubSystem_Imp.h"

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
				UInputProcessorSubSystem_Imp::GetInstance()->GetCurrentAction()
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
				UInputProcessorSubSystem_Imp::GetInstance()->GetCurrentAction()
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
	const TSubclassOf<AGuideInteractionBase>& GuideInteractionClass
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
	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
}
