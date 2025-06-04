#include "GuideSystemGameplayTask.h"

#include "Kismet/GameplayStatics.h"

#include "AssetRefMap.h"
#include "HumanCharacter_Player.h"
#include "ResourceBoxBase.h"
#include "SceneActor.h"
#include "STT_GuideThread.h"
#include "ChallengeEntry.h"
#include "GameplayTagsLibrary.h"
#include "GroupManagger.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "MainMenuLayout.h"
#include "PlanetPlayerController.h"
#include "TargetPoint_Runtime.h"
#include "TeamMatesHelperComponent.h"
#include "TeamMatesHelperComponentBase.h"
#include "UIManagerSubSystem.h"

void UGameplayTask_Base::SetPlayerCharacter(
	AHumanCharacter_Player* InPlayerCharacterPtr
	)
{
	PlayerCharacterPtr = InPlayerCharacterPtr;
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

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = PlayerCharacterPtr;

	auto PADPtr = PAD.LoadSynchronous();
	if (PADPtr)
	{
		TObjectPtr<AActor> TargetActor = nullptr;
		if (PADPtr->ResourceBoxClass)
		{
		}
		else if (auto CharacterPtr = PADPtr->CharacterPtr.LoadSynchronous())
		{
			TargetActor = CharacterPtr;
		}
		else if (auto ResourceBoxPtr = PADPtr->ResourceBoxPtr.LoadSynchronous())
		{
			TargetActor = ResourceBoxPtr;
		}
		else if (auto ChallengeEntryPtr = PADPtr->ChallengeEntryPtr.LoadSynchronous())
		{
			TargetActor = ChallengeEntryPtr;
		}

		if (TargetActor)
		{
			TargetPointPtr = PlayerCharacterPtr->GetWorld()->SpawnActor<
				ATargetPoint_Runtime>(
				                      TargetPoint_RuntimeClass,
				                      SpawnParameters
				                     );

			TargetPointPtr->AttachToActor(
			                              TargetActor,
			                              FAttachmentTransformRules::KeepRelativeTransform
			                             );
		}
	}
}

void UGameplayTask_WaitInteractionSceneActor::OnDestroy(
	bool bInOwnerFinished
	)
{
	PlayerCharacterPtr->GetSceneCharacterPlayerInteractionComponent()->OnPlayerInteraction.Remove(DelegateHandle);

	if (TargetPointPtr)
	{
		TargetPointPtr->Destroy();
	}
	TargetPointPtr = nullptr;

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_WaitInteractionSceneActor::OnInteractionSceneActor(
	ISceneActorInteractionInterface* TargetActorPtr
	)
{
	auto PADPtr = PAD.LoadSynchronous();
	if (PADPtr && TargetActorPtr)
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
		MemberChangedDelegate.Reset();
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_WaitPlayerEquipment::OnCharacterSocketUpdated(
	const FCharacterSocket& Socket,
	const FGameplayTag& ProxyType
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

UGameplayTask_WaitOpenLayout::UGameplayTask_WaitOpenLayout(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_WaitOpenLayout::Activate()
{
	Super::Activate();

	OnSwitchToLayout = UUIManagerSubSystem::GetInstance()->GetMainHUDLayout()->OnLayoutChanged.AddCallback(
		 std::bind(&ThisClass::OnSwitchToNewLayout, this, std::placeholders::_1)
		);

	switch (TargetLayoutCommon)
	{
	case ELayoutCommon::kMenuLayout:
		{
			OnSwitchToMenuLayout = UUIManagerSubSystem::GetInstance()->GetMainMenuLayout()->OnMenuLayoutChanged.
			                                                           AddCallback(
				                                                            std::bind(
					                                                             &ThisClass::OnSwitchToNewMenuLayout,
					                                                             this,
					                                                             std::placeholders::_1
					                                                            )
				                                                           );
		}
		break;
	}
}

void UGameplayTask_WaitOpenLayout::OnDestroy(
	bool bInOwnerFinished
	)
{
	if (OnSwitchToLayout)
	{
		OnSwitchToLayout.Reset();
	}
	if (OnSwitchToMenuLayout)
	{
		OnSwitchToMenuLayout.Reset();
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_WaitOpenLayout::OnSwitchToNewLayout(
	ELayoutCommon Layout
	)
{
	switch (TargetLayoutCommon)
	{
	case ELayoutCommon::kMenuLayout:
		{
		}
		break;
	default:
		{
			if (Layout == TargetLayoutCommon)
			{
				EndTask();
			}
		}
	}
}

void UGameplayTask_WaitOpenLayout::OnSwitchToNewMenuLayout(
	EMenuType MenuType
	)
{
	if (MenuType == TargetMenuType)
	{
		EndTask();
	}
}
