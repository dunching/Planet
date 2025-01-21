#include "ConversationLayout.h"

#include "GuideActor.h"
#include "GuideInteractionActor.h"
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

	if (GuideInteractionActorPtr)
	{
		GuideInteractionActorPtr->OnGuideInteractionEnd.Clear();
		GuideInteractionActorPtr->GetGuideSystemStateTreeComponent()->Cleanup();
		GuideInteractionActorPtr = nullptr;
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

void UConversationLayout::SelectedInteractionItem(const TSubclassOf<AGuideInteractionActor>& GuideInteractionClass)
{
	auto CurrentActionSPtr =
		DynamicCastSharedPtr<HumanProcessor::FHumanInteractionWithNPCProcessor>(
			UInputProcessorSubSystem::GetInstance()->GetCurrentAction()
		);
	if (CurrentActionSPtr)
	{
		FActorSpawnParameters SpawnParameters;

		GuideInteractionActorPtr = GetWorld()->SpawnActor<AGuideInteractionActor>(
			GuideInteractionClass, SpawnParameters
		);

		if (GuideInteractionActorPtr)
		{
			auto PlayerCharacterPtr = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(this, 0));

			GuideInteractionActorPtr->PlayerCharacter = PlayerCharacterPtr;
		}
		GuideInteractionActorPtr->TargetCharacter = CurrentActionSPtr->CharacterPtr;

		GuideInteractionActorPtr->OnGuideInteractionEnd.AddLambda([]
		{
			UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
		});

		Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(this, 0))
			->GetHUD<AMainHUD>()
			->GetMainHUDLayout()
			->GetConversationLayout()
			->CloseOption();
	}
}
