#include "InteractionConversationLayout.h"

#include "GuideActor.h"
#include "GuideInteraction.h"
#include "GuideSystemStateTreeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"

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

	FName QuitBtn = TEXT("QuitBtn");
};

void UInteractionConversationLayout::NativeConstruct()
{
	Super::NativeConstruct();

	{
		auto UIPtr = Cast<UButton>(
			GetWidgetFromName(FConversationLayout::Get().QuitBtn)
		);
		if (UIPtr)
		{
			UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnQuitBtnClicked);
		}
	}

	Enable();
}

void UInteractionConversationLayout::Enable()
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
}

ELayoutCommon UInteractionConversationLayout::GetLayoutType() const
{
	return ELayoutCommon::kConversationLayout;
}

void UInteractionConversationLayout::OnQuitBtnClicked()
{
	UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
}
