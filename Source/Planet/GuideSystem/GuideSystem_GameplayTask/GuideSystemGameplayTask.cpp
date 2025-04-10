#include "GuideSystemGameplayTask.h"

#include "Kismet/GameplayStatics.h"

#include "AssetRefMap.h"
#include "ConversationLayout.h"
#include "GuideActor.h"
#include "GuideSubSystem.h"
#include "HumanCharacter_AI.h"
#include "HumanCharacter_Player.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "OptionList.h"
#include "PlanetPlayerController.h"
#include "ResourceBoxBase.h"
#include "SceneActor.h"
#include "STT_GuideThread.h"
#include "TargetPoint_Runtime.h"


class AMainHUD;

void UGameplayTask_Base::SetPlayerCharacter(AHumanCharacter_Player* InPlayerCharacterPtr)
{
	PlayerCharacterPtr = InPlayerCharacterPtr;
}

void UGameplayTask_Base::SetTaskID(const FGuid& InTaskID)
{
	TaskID = InTaskID;
}


EStateTreeRunStatus UGameplayTask_Base::GetStateTreeRunStatus() const
{
	return StateTreeRunStatus;
}

UGameplayTask_WaitInteractionSceneActor::UGameplayTask_WaitInteractionSceneActor(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_WaitInteractionSceneActor::Activate()
{
	Super::Activate();

	DelegateHandle = PlayerCharacterPtr->GetSceneCharacterPlayerInteractionComponent()->OnPlayerInteraction.AddUObject(
		this, &ThisClass::OnInteractionSceneActor
	);
}

void UGameplayTask_WaitInteractionSceneActor::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (bIsInteractionSceneActor)
	{
		EndTask();
	}
}

void UGameplayTask_WaitInteractionSceneActor::OnDestroy(bool bInOwnerFinished)
{
	PlayerCharacterPtr->GetSceneCharacterPlayerInteractionComponent()->OnPlayerInteraction.Remove(DelegateHandle);

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_WaitInteractionSceneActor::OnInteractionSceneActor(ASceneActor* TargetActorPtr)
{
	auto PADPtr = PAD.LoadSynchronous();
	if (TargetActorPtr)
	{
		if (TargetActorPtr->IsA(PADPtr->ResourceBoxClass))
		{
			bIsInteractionSceneActor = true;
		}
		else if (auto CharacterPtr = PADPtr->CharacterPtr.LoadSynchronous())
		{
		}
	}
}

UGameplayTask_WaitPlayerEquipment::UGameplayTask_WaitPlayerEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_WaitPlayerEquipment::Activate()
{
	Super::Activate();

	DelegateHandle = PlayerCharacterPtr->GetCharacterProxy()->OnCharacterSocketUpdated.AddUObject(
		this, &ThisClass::OnCharacterSocketUpdated
	);
}

void UGameplayTask_WaitPlayerEquipment::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
}

void UGameplayTask_WaitPlayerEquipment::OnDestroy(bool bInOwnerFinished)
{
	PlayerCharacterPtr->GetSceneCharacterPlayerInteractionComponent()->OnPlayerInteraction.Remove(DelegateHandle);

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_WaitPlayerEquipment::OnCharacterSocketUpdated(const FCharacterSocket& Socket)
{
	if (Socket.IsValid())
	{
		if (Socket.Socket.MatchesTag(WeaponSocket))
		{
			EndTask();
		}
		else if (Socket.Socket.MatchesTag(SkillSocket))
		{
			EndTask();
		}
	}
}
