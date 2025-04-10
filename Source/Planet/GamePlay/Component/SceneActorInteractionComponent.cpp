#include "SceneActorInteractionComponent.h"

#include "ConversationLayout.h"
#include "Kismet/GameplayStatics.h"

#include "HumanCharacter_AI.h"
#include "GuideInteraction.h"
#include "HumanCharacter_Player.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "PlanetPlayerController.h"

class AMainHUD;
FName USceneActorInteractionComponent::ComponentName = TEXT("SceneActorInteractionComponent");

USceneActorInteractionComponent::USceneActorInteractionComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	// 因为有切换任务 所以这里我们不要他自动调用，
	// bStartLogicAutomatically = false;
}

TArray<TSubclassOf<AGuideInteraction_Actor>> USceneActorInteractionComponent::GetInteractionLists() const
{
	TArray<TSubclassOf<AGuideInteraction_Actor>> Results;

	for (const auto& Iter : GuideInteractionAry)
	{
		if (Iter.bIsEnable)
		{
			Results.Add(Iter.GuideInteraction);
		}
	}

	return Results;
}

void USceneActorInteractionComponent::StartInteractionItem(const TSubclassOf<AGuideInteraction_Actor>& Item)
{
}

void USceneActorInteractionComponent::StopInteractionItem()
{
	if (GuideInteractionActorPtr)
	{
		// GuideInteractionActorPtr->GetGuideSystemStateTreeComponent()->Cleanup();
		GuideInteractionActorPtr->bWantToStop = true;
		GuideInteractionActorPtr = nullptr;
	}
}

void USceneActorInteractionComponent::ChangedInterationState(
	const TSubclassOf<AGuideInteraction_Actor>& Item,
	bool bIsEnable
)
{
	for (auto& Iter : GuideInteractionAry)
	{
		if (Iter.GuideInteraction == Item)
		{
			Iter.bIsEnable = bIsEnable;
			break;
		}
	}
}
