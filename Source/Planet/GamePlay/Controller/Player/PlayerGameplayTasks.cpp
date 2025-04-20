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

void UPlayerControllerGameplayTasksComponent::TeleportPlayerToNearest_Server_Implementation()
{
	auto OwnerPtr = GetOwner<FOwnerType>();
	if (!OwnerPtr)
	{
		return;
	}

	auto Teleport = UOpenWorldSubSystem::GetInstance()->GetTeleportPlayerToNearest(OwnerPtr);
	
	TeleportPlayerToNearest_Task(Teleport);
}

 void UPlayerControllerGameplayTasksComponent::TeleportPlayerToNearest_Task_Implementation(ETeleport Teleport)
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
		if (CharacterPtr )
		{
			FGameplayTagContainer FGameplayTagContainer(UGameplayTagsLibrary::BaseFeature_Dying);
			CharacterPtr ->GetCharacterAbilitySystemComponent()->CancelAbilities(&FGameplayTagContainer);
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

	Target = UOpenWorldSubSystem::GetInstance()->GetTeleport(Teleport);
	
	Target.LoadSynchronous();
}

void UGameplayTask_TeleportPlayer::TickTask(
	float DeltaTime
)
{
	Super::TickTask(DeltaTime);

	const auto Distance = FVector::Dist2D(Target->GetLandTransform().GetLocation(), TargetPCPtr->GetPawn()->GetActorLocation());
	if (Distance < DistanceThreshold)
	{
		bIsSuccessful = true;
		EndTask();
	}
}

void UGameplayTask_TeleportPlayer::OnDestroy(
	bool bInOwnerFinished
)
{
	OnEnd.Broadcast(bIsSuccessful);
	
	Super::OnDestroy(bInOwnerFinished);
}
