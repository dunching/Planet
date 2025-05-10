#include "SceneActorInteractionComponent.h"

#include "Kismet/GameplayStatics.h"

#include "HumanCharacter_AI.h"
#include "GuideInteraction.h"
#include "GuideSubSystem.h"
#include "HumanCharacter_Player.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "PlanetPlayerController.h"

class AMainHUD;
FName USceneActorInteractionComponent::ComponentName = TEXT("SceneActorInteractionComponent");

USceneActorInteractionComponent::USceneActorInteractionComponent(
	const FObjectInitializer& ObjectInitializer
	):
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

TObjectPtr<AGuideInteraction_Actor> USceneActorInteractionComponent::GetCurrentInteraction() const
{
	return GuideInteractionActorPtr;
}

TArray<FGuideInterationSetting> USceneActorInteractionComponent::GetGuideInteractionAry() const
{
	return GuideInteractionAry;
}

void USceneActorInteractionComponent::StartInteractionItem(
	const TSubclassOf<AGuideInteraction_Actor>& Item
	)
{
}

void USceneActorInteractionComponent::StopInteractionItem()
{
	if (GuideInteractionActorPtr)
	{
		UGuideSubSystem::GetInstance()->AddGuidePostion(
		                                                GuideInteractionActorPtr->GetGuideID(),
		                                                GuideInteractionActorPtr->GetCurrentTaskID()
		                                               );

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

bool USceneActorInteractionComponent::GetIsEnableInteraction() const
{
	return bEnableInteraction && !GetInteractionLists().IsEmpty();
}

void USceneActorInteractionComponent::StartInteractionImp(
	const TSubclassOf<AGuideInteraction_Actor>& Item,
	const TObjectPtr<AGuideInteraction_Actor>& GuideInteraction_ActorPtr
	)
{
	GuideInteractionActorPtr = GuideInteraction_ActorPtr;
	
	const auto NewGuideID = Item.GetDefaultObject()->GetGuideID();
	if (!NewGuideID.IsValid())
	{
		checkNoEntry();
	}
	
	FGuid CurrentTaskID;
	if (UGuideSubSystem::GetInstance()->ConsumeGuidePostion(NewGuideID, CurrentTaskID))
	{
		//
		GuideInteractionActorPtr->SetPreviousTaskID(CurrentTaskID);
	}

	GuideInteractionActorPtr->GetGuideSystemStateTreeComponent()->StartLogic();
}
