#include "PlayerGameplayTasks.h"

#include "CharacterAbilitySystemComponent.h"
#include "Dynamic_Weather.h"
#include "GameplayTagsLibrary.h"
#include "HumanRegularProcessor.h"
#include "InputProcessorSubSystemBase.h"
#include "OpenWorldSystem.h"
#include "PlanetPlayerController.h"
#include "Teleport.h"
#include "HumanCharacter_Player.h"
#include "InputProcessorSubSystem_Imp.h"
#include "LogWriter.h"
#include "TransitionProcessor.h"
#include "WeatherSystem.h"

FName UPlayerControllerGameplayTasksComponent::ComponentName = TEXT("PlayerControllerGameplayTasksComponent");

void UPlayerControllerGameplayTasksComponent::TeleportPlayerToNearest()
{
	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<HumanProcessor::FTransitionProcessor>();

	TeleportPlayerToNearest_Server();
}

void UPlayerControllerGameplayTasksComponent::EntryChallengeLevel(
	ETeleport Teleport
	)
{
	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<HumanProcessor::FTransitionProcessor>();

	EntryChallengeLevel_Server(Teleport);
}

void UPlayerControllerGameplayTasksComponent::TeleportPlayerToOpenWorld_Server_Implementation()
{
	auto OwnerPtr = GetOwner<FOwnerType>();
	if (!OwnerPtr)
	{
		return;
	}

	auto Teleport = UOpenWorldSubSystem::GetInstance()->GetTeleportLastPtInOpenWorld(OwnerPtr);

	EntryLevel_ActiveTask(Teleport, OpenWorldWeather);
}

void UPlayerControllerGameplayTasksComponent::TeleportPlayerToNearest_Server_Implementation()
{
	auto OwnerPtr = GetOwner<FOwnerType>();
	if (!OwnerPtr)
	{
		return;
	}

	auto Teleport = UOpenWorldSubSystem::GetInstance()->GetTeleportPlayerToNearest(OwnerPtr);

	FGameplayTag NewWeather = UOpenWorldSubSystem::GetInstance()->GetTeleportWeather(Teleport);

	EntryLevel_ActiveTask(Teleport, NewWeather);
}

void UPlayerControllerGameplayTasksComponent::EntryChallengeLevel_Server_Implementation(
	ETeleport Teleport
	)
{
	FGameplayTag NewWeather = UOpenWorldSubSystem::GetInstance()->GetTeleportWeather(Teleport);

	EntryLevel_ActiveTask(Teleport, NewWeather);
}

void UPlayerControllerGameplayTasksComponent::EntryLevel_ActiveTask_Implementation(
	ETeleport Teleport,
	const FGameplayTag& NewWeather
	)
{
	auto OwnerPtr = GetOwner<FOwnerType>();
	if (!OwnerPtr)
	{
		return;
	}

	auto GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_TeleportPlayer>(
																				TScriptInterface<
																					IGameplayTaskOwnerInterface>(
																					 this
																					)
																			   );
	GameplayTaskPtr->Teleport = Teleport;
	GameplayTaskPtr->TargetPCPtr = OwnerPtr;
	GameplayTaskPtr->Weather = NewWeather;
	GameplayTaskPtr->OnEnd.AddUObject(this, &ThisClass::EntryLevelEnd);

	GameplayTaskPtr->ReadyForActivation();
}

void UPlayerControllerGameplayTasksComponent::EntryLevelEnd(
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
		UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
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

void UPlayerControllerGameplayTasksComponent::WaitPlayerLoad()
{
	auto OwnerPtr = GetOwner<FOwnerType>();
	if (!OwnerPtr)
	{
		return;
	}

	auto GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_WaitLoadComplete>(
																				TScriptInterface<
																					IGameplayTaskOwnerInterface>(
																					 this
																					)
																			   );
	GameplayTaskPtr->TargetPCPtr = OwnerPtr;

	GameplayTaskPtr->ReadyForActivation();
}

void UPlayerControllerGameplayTasksComponent::TeleportPlayerToOpenWorld()
{
	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<HumanProcessor::FTransitionProcessor>();

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
	
	Target.LoadSynchronous();
	
#if UE_EDITOR || UE_SERVER
	if (TargetPCPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (Weather.IsValid())
		{
			TargetPCPtr->GetGameplayTasksComponent()->OpenWorldWeather = UWeatherSystem::GetInstance()->GetDynamicWeather()
				->GetCurrentWeather();
	
			// 改变天气
			UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateWeather(Weather);
		}
		
		UOpenWorldSubSystem::GetInstance()->SwitchDataLayer(Teleport, TargetPCPtr);
	}

	// 
	auto CharacterPtr = Cast<ACharacterBase>(TargetPCPtr->GetPawn<ACharacterBase>());
	if (CharacterPtr)
	{
		CharacterPtr->LandedDelegate.AddDynamic(this, &ThisClass::OnLanded);
	}
#endif

	if (TargetPCPtr->GetLocalRole() > ROLE_SimulatedProxy)
	{
	}
}

void UGameplayTask_TeleportPlayer::TickTask(
	float DeltaTime
	)
{
	Super::TickTask(DeltaTime);

	if (CurrentWaitTime < MinWaitTime)
	{
		CurrentWaitTime+=DeltaTime;
	}
	
	if (bIsSwitchDataLayerComplete)
	{
		if (bIsSuccessful)
		{
			if (CurrentWaitTime < MinWaitTime)
			{
				PRINTINVOKEWITHSTR(FString(TEXT("Not Yet")));
				return;	
			}
			
			if ((CurrentWaitTime >= MinWaitTime) && bIsOnLanded)
			{
#if UE_EDITOR || UE_CLIENT
				if (TargetPCPtr->GetLocalRole() == ROLE_AutonomousProxy)
				{
				}
#endif

#if UE_EDITOR || UE_SERVER
				if (TargetPCPtr->GetLocalRole() == ROLE_Authority)
				{
				}
#endif

				EndTask();
			}
		}
		else if (UOpenWorldSubSystem::GetInstance()->CheckTeleportPlayerComplete(Teleport))
		{ 
			bIsSuccessful = true;
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
	auto CharacterPtr = Cast<ACharacterBase>(TargetPCPtr->GetPawn<ACharacterBase>());
	if (CharacterPtr)
	{
		CharacterPtr->LandedDelegate.RemoveDynamic(this, &ThisClass::OnLanded);
	}
	
	OnEnd.Broadcast(bIsSuccessful);

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_TeleportPlayer::OnLanded(
	const FHitResult& Hit
	)
{
	bIsOnLanded = true;
}

inline UGameplayTask_WaitLoadComplete::UGameplayTask_WaitLoadComplete(
	const FObjectInitializer& ObjectInitializer
	):Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_WaitLoadComplete::Activate()
{
	Super::Activate();

	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<HumanProcessor::FTransitionProcessor>();

	auto CharacterPtr = Cast<ACharacterBase>(TargetPCPtr->GetPawn<ACharacterBase>());
	if (CharacterPtr)
	{
		CharacterPtr->LandedDelegate.AddDynamic(this, &ThisClass::OnLanded);
	}
}

void UGameplayTask_WaitLoadComplete::TickTask(
	float DeltaTime
	)
{
	Super::TickTask(DeltaTime);
	if (CurrentWaitTime < MinWaitTime)
	{
		CurrentWaitTime+=DeltaTime;
	}
	
	if ((CurrentWaitTime >= MinWaitTime) && bIsOnLanded)
	{
		OnEnd.Broadcast(true);
		
		EndTask();
	}
}

void UGameplayTask_WaitLoadComplete::OnDestroy(
	bool bInOwnerFinished
	)
{
	auto CharacterPtr = Cast<ACharacterBase>(TargetPCPtr->GetPawn<ACharacterBase>());
	if (CharacterPtr)
	{
		CharacterPtr->LandedDelegate.RemoveDynamic(this, &ThisClass::OnLanded);
	}
	
	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_WaitLoadComplete::OnLanded(
	const FHitResult& Hit
	)
{
	bIsOnLanded = true;
}
