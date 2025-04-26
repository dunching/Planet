#include "GuideSystemGameplayTask.h"

#include "AssetRefMap.h"
#include "HumanCharacter_Player.h"
#include "ResourceBoxBase.h"
#include "SceneActor.h"
#include "STT_GuideThread.h"
#include "ChallengeEntry.h"
#include "GameplayTagsLibrary.h"
#include "GroupManagger.h"
#include "TeamMatesHelperComponent.h"


class AMainHUD;

void UGameplayTask_Base::SetPlayerCharacter(
	AHumanCharacter_Player* InPlayerCharacterPtr
)
{
	PlayerCharacterPtr = InPlayerCharacterPtr;
}

void UGameplayTask_Base::SetTaskID(
	const FGuid& InTaskID
)
{
	TaskID = InTaskID;
}


EStateTreeRunStatus UGameplayTask_Base::GetStateTreeRunStatus() const
{
	return StateTreeRunStatus;
}

UGameplayTask_WaitInteractionSceneActor::UGameplayTask_WaitInteractionSceneActor(
	const FObjectInitializer& ObjectInitializer
) :
  Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_WaitInteractionSceneActor::Activate()
{
	Super::Activate();

	DelegateHandle = PlayerCharacterPtr->GetSceneCharacterPlayerInteractionComponent()->OnPlayerInteraction.AddUObject(
		this,
		&ThisClass::OnInteractionSceneActor
	);
}

void UGameplayTask_WaitInteractionSceneActor::OnDestroy(
	bool bInOwnerFinished
)
{
	PlayerCharacterPtr->GetSceneCharacterPlayerInteractionComponent()->OnPlayerInteraction.Remove(DelegateHandle);

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_WaitInteractionSceneActor::OnInteractionSceneActor(
	ISceneActorInteractionInterface* TargetActorPtr
)
{
	auto PADPtr = PAD.LoadSynchronous();
	if (TargetActorPtr)
	{
		if (PADPtr->ResourceBoxClass)
		{
			auto ResourceBoxPtr = Cast<AResourceBoxBase>(TargetActorPtr);
			if (ResourceBoxPtr && ResourceBoxPtr->IsA(PADPtr->ResourceBoxClass))
			{
				EndTask();
			}
		}
		else if (auto CharacterPtr = PADPtr->CharacterPtr.LoadSynchronous())
		{
			if (CharacterPtr == TargetActorPtr)
			{
				EndTask();
			}
		}
		else if (auto ResourceBoxPtr = PADPtr->ResourceBoxPtr.LoadSynchronous())
		{
			if (ResourceBoxPtr == TargetActorPtr)
			{
				EndTask();
			}
		}
		else if (auto ChallengeEntryPtr = PADPtr->ChallengeEntryPtr.LoadSynchronous())
		{
			if (ChallengeEntryPtr == TargetActorPtr)
			{
				EndTask();
			}
		}
	}
}

UGameplayTask_WaitPlayerEquipment::UGameplayTask_WaitPlayerEquipment(
	const FObjectInitializer& ObjectInitializer
) :
  Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_WaitPlayerEquipment::Activate()
{
	Super::Activate();

	if (bIsEquipentCharacter)
	{
		MemberChangedDelegate = PlayerCharacterPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->MembersChanged.
		                                            AddCallback(
			                                            std::bind(
				                                            &ThisClass::OnMembersChanged,
				                                            this,
				                                            std::placeholders::_1,
				                                            std::placeholders::_2
			                                            )
		                                            );
	}
	else
	{
		DelegateHandle = PlayerCharacterPtr->GetCharacterProxy()->OnCharacterSocketUpdated.AddUObject(
			this,
			&ThisClass::OnCharacterSocketUpdated
		);
	}
}

void UGameplayTask_WaitPlayerEquipment::TickTask(
	float DeltaTime
)
{
	Super::TickTask(DeltaTime);
}

void UGameplayTask_WaitPlayerEquipment::OnDestroy(
	bool bInOwnerFinished
)
{
	PlayerCharacterPtr->GetSceneCharacterPlayerInteractionComponent()->OnPlayerInteraction.Remove(DelegateHandle);

	if (MemberChangedDelegate)
	{
		MemberChangedDelegate->UnBindCallback();
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_WaitPlayerEquipment::OnCharacterSocketUpdated(
	const FCharacterSocket& Socket,const FGameplayTag&ProxyType
)
{
	if (bPlayerAssign)
	{
		if (!UGameplayTagsLibrary::Proxy_Character_Player.MatchesTag(ProxyType))
		{
			return;
		}
	}
	else
	{
		if (!UGameplayTagsLibrary::Proxy_Character_NPC_Assistional.MatchesTag(ProxyType))
		{
			return;
		}
	}
	
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

void UGameplayTask_WaitPlayerEquipment::OnMembersChanged(
	const FTeammate& Teammate,
	const TSharedPtr<FCharacterProxy>& CharacterProxySPtr
)
{
	if (CharacterProxySPtr)
	{
		EndTask();
	}
}
