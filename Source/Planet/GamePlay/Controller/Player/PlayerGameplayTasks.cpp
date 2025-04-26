#include "PlayerGameplayTasks.h"

#include "CharacterAbilitySystemComponent.h"
#include "GameplayTagsLibrary.h"
#include "HumanRegularProcessor.h"
#include "InputProcessorSubSystem.h"
#include "OpenWorldSystem.h"
#include "PlanetPlayerController.h"
#include "Teleport.h"
#include "HumanCharacter_Player.h"
#include "TransitionProcessor.h"

FName UPlayerControllerGameplayTasksComponent::ComponentName = TEXT("PlayerControllerGameplayTasksComponent");

void UPlayerControllerGameplayTasksComponent::TeleportPlayerToNearest()
{
	UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FTransitionProcessor>();

	TeleportPlayerToNearest_Server();
}

void UPlayerControllerGameplayTasksComponent::EntryChallengeLevel(
	ETeleport Teleport
)
{
	UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FTransitionProcessor>();

	EntryChallengeLevel_Server(Teleport);
}

void UPlayerControllerGameplayTasksComponent::TeleportPlayerToOpenWorldEnd(
	bool bIsSuccess
)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto OwnerPtr = GetOwner<FOwnerType>();
		auto CharacterPtr = OwnerPtr->GetPawn<AHumanCharacter_Player>();
		if (CharacterPtr)
		{
			FGameplayTagContainer FGameplayTagContainer(UGameplayTagsLibrary::BaseFeature_Dying);
			CharacterPtr->GetCharacterAbilitySystemComponent()->CancelAbilities(&FGameplayTagContainer);
		}
	}
#endif

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
	}
#endif
}

void UPlayerControllerGameplayTasksComponent::TeleportPlayerToOpenWorld_ActiveTask_Implementation(
	ETeleport Teleport
)
{
	auto OwnerPtr = GetOwner<FOwnerType>();
	if (!OwnerPtr)
	{
		return;
	}

	auto GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_TeleportPlayer>(
		TScriptInterface<IGameplayTaskOwnerInterface>(
			this
		)
	);
	GameplayTaskPtr->Teleport = Teleport;
	GameplayTaskPtr->TargetPCPtr = OwnerPtr;
	GameplayTaskPtr->OnEnd.AddUObject(this, &ThisClass::TeleportPlayerToOpenWorldEnd);

	GameplayTaskPtr->ReadyForActivation();
}

void UPlayerControllerGameplayTasksComponent::TeleportPlayerToOpenWorld_Server_Implementation()
{
	auto OwnerPtr = GetOwner<FOwnerType>();
	if (!OwnerPtr)
	{
		return;
	}

	auto Teleport = UOpenWorldSubSystem::GetInstance()->GetTeleportLastPtInOpenWorld(OwnerPtr);

	TeleportPlayerToOpenWorld_ActiveTask(Teleport);
}

void UPlayerControllerGameplayTasksComponent::TeleportPlayerToNearest_Server_Implementation()
{
	auto OwnerPtr = GetOwner<FOwnerType>();
	if (!OwnerPtr)
	{
		return;
	}

	auto Teleport = UOpenWorldSubSystem::GetInstance()->GetTeleportPlayerToNearest(OwnerPtr);

	TeleportPlayerToNearest_ActiveTask(Teleport);
}

void UPlayerControllerGameplayTasksComponent::TeleportPlayerToNearest_ActiveTask_Implementation(
	ETeleport Teleport
)
{
	auto OwnerPtr = GetOwner<FOwnerType>();
	if (!OwnerPtr)
	{
		return;
	}

	auto GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_TeleportPlayer>(
		TScriptInterface<IGameplayTaskOwnerInterface>(
			this
		)
	);
	GameplayTaskPtr->Teleport = Teleport;
	GameplayTaskPtr->TargetPCPtr = OwnerPtr;
	GameplayTaskPtr->OnEnd.AddUObject(this, &ThisClass::TeleportPlayerToNearestEnd);

	GameplayTaskPtr->ReadyForActivation();
}

void UPlayerControllerGameplayTasksComponent::TeleportPlayerToNearestEnd(
	bool bIsSuccess
)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto OwnerPtr = GetOwner<FOwnerType>();
		auto CharacterPtr = OwnerPtr->GetPawn<AHumanCharacter_Player>();
		if (CharacterPtr)
		{
			FGameplayTagContainer FGameplayTagContainer(UGameplayTagsLibrary::BaseFeature_Dying);
			CharacterPtr->GetCharacterAbilitySystemComponent()->CancelAbilities(&FGameplayTagContainer);
		}
	}
#endif

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
	}
#endif
}

void UPlayerControllerGameplayTasksComponent::EntryChallengeLevel_Server_Implementation(
	ETeleport Teleport
)
{
	EntryChallengeLevel_ActiveTask(Teleport);
}

void UPlayerControllerGameplayTasksComponent::EntryChallengeLevel_ActiveTask_Implementation(
	ETeleport Teleport
)
{
	auto OwnerPtr = GetOwner<FOwnerType>();
	if (!OwnerPtr)
	{
		return;
	}

	auto GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_TeleportPlayer>(
		TScriptInterface<IGameplayTaskOwnerInterface>(
			this
		)
	);
	GameplayTaskPtr->Teleport = Teleport;
	GameplayTaskPtr->TargetPCPtr = OwnerPtr;
	GameplayTaskPtr->OnEnd.AddUObject(this, &ThisClass::EntryChallengeLevelEnd);

	GameplayTaskPtr->ReadyForActivation();
}

void UPlayerControllerGameplayTasksComponent::EntryChallengeLevelEnd(
	bool bIsSuccess
)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto OwnerPtr = GetOwner<FOwnerType>();
		auto CharacterPtr = OwnerPtr->GetPawn<AHumanCharacter_Player>();
		if (CharacterPtr)
		{
			FGameplayTagContainer FGameplayTagContainer(UGameplayTagsLibrary::BaseFeature_Dying);
			CharacterPtr->GetCharacterAbilitySystemComponent()->CancelAbilities(&FGameplayTagContainer);
		}
	}
#endif

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
	}
#endif
}

UPlayerControllerGameplayTasksComponent::UPlayerControllerGameplayTasksComponent(
	const FObjectInitializer& ObjectInitializer
) :
  Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UPlayerControllerGameplayTasksComponent::TeleportPlayerToOpenWorld()
{
	UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FTransitionProcessor>();

	TeleportPlayerToOpenWorld_Server();
}

UGameplayTask_TeleportPlayer::UGameplayTask_TeleportPlayer(
	const FObjectInitializer& ObjectInitializer
):
 Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_TeleportPlayer::Activate()
{
	Super::Activate();

#if UE_EDITOR || UE_SERVER
	if (TargetPCPtr->GetNetMode() == NM_DedicatedServer)
	{
		UOpenWorldSubSystem::GetInstance()->SwitchDataLayer(Teleport, TargetPCPtr);
	}
#endif

	Target.LoadSynchronous();
}

void UGameplayTask_TeleportPlayer::TickTask(
	float DeltaTime
)
{
	Super::TickTask(DeltaTime);

	if (bIsSwitchDataLayerComplete)
	{
		if (UOpenWorldSubSystem::GetInstance()->CheckTeleportPlayerComplete(Teleport))
		{
			bIsSuccessful = true;
			EndTask();
		}
	}
	else
	{
		if (UOpenWorldSubSystem::GetInstance()->CheckSwitchDataLayerComplete(Teleport))
		{
			bIsSwitchDataLayerComplete = true;

#if UE_EDITOR || UE_SERVER
			if (TargetPCPtr->GetNetMode() == NM_DedicatedServer)
			{
				UOpenWorldSubSystem::GetInstance()->TeleportPlayer(Teleport, TargetPCPtr);
			}
#endif
		}
	}
}

void UGameplayTask_TeleportPlayer::OnDestroy(
	bool bInOwnerFinished
)
{
	OnEnd.Broadcast(bIsSuccessful);

	Super::OnDestroy(bInOwnerFinished);
}
