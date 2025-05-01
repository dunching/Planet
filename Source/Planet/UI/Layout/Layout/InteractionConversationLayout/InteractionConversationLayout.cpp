#include "InteractionConversationLayout.h"

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

void UInteractionConversationLayout::NativeConstruct()
{
	Super::NativeConstruct();

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
