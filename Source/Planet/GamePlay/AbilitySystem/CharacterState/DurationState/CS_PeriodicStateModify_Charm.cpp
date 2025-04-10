
#include "CS_PeriodicStateModify_Charm.h"

#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>
#include "AbilitySystemGlobals.h"
#include <GameFramework/CharacterMovementComponent.h>

#include "KismetGravityLibrary.h"

#include "AbilityTask_TimerHelper.h"
#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"

#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "CharacterAbilitySystemComponent.h"
#include "GameplayTagsLibrary.h"
#include "AbilityTask_ARM_ConstantForce.h"
#include "AbilityTask_FlyAway.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "StateProcessorComponent.h"
#include "CharacterStateInfo.h"
#include "PlanetPlayerController.h"
#include "PlanetAIController.h"

FGameplayAbilityTargetData_StateModify_Charm::FGameplayAbilityTargetData_StateModify_Charm(
	float Duration
) :
	Super(UGameplayTagsLibrary::State_Debuff_Charm, Duration)
{
}

FGameplayAbilityTargetData_StateModify_Charm::FGameplayAbilityTargetData_StateModify_Charm()
{

}

UCS_PeriodicStateModify_Charm::UCS_PeriodicStateModify_Charm() :
	Super()
{
}

void UCS_PeriodicStateModify_Charm::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UCS_PeriodicStateModify_Charm::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UCS_PeriodicStateModify_Charm::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	//
	//
	if (auto PCPtr = CharacterPtr->GetController<APlanetPlayerController>())
	{
		PCPtr->ReceiveMoveCompleted.Unbind();
		PCPtr->StopMovement();
	}
	else if (auto AICPtr = CharacterPtr->GetController<APlanetAIController>())
	{
		AICPtr->ReceiveMoveCompleted.RemoveDynamic(this, &ThisClass::OnMoveCompleted);
		AICPtr->StopMovement();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCS_PeriodicStateModify_Charm::UpdateDuration()
{
	Super::UpdateDuration();

	PerformAction();
}

// void UCS_PeriodicStateModify_Charm::InitalDefaultTags()
// {
// 	Super::InitalDefaultTags();
//
// 	// AbilityTags.AddTag(UGameplayTagsLibrary::State_Debuff_Charm);
// 	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::State_Debuff_Charm);
//
// 	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantPlayerInputMove);
// 	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantJump);
// 	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantRootMotion);
// 	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantRotation);;
// 	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_Orient2Acce);
// }

void UCS_PeriodicStateModify_Charm::PerformAction()
{
	// 
	//
	if (auto PCPtr = CharacterPtr->GetController<APlanetPlayerController>())
	{
		PCPtr->MoveToLocation(
			FVector::ZeroVector,
			GameplayAbilityTargetDataSPtr->TriggerCharacterPtr.Get(),
			AcceptanceRadius
		);
		PCPtr->ReceiveMoveCompleted.BindUObject(this, &ThisClass::OnMoveCompleted);
	}
	else if (auto AICPtr = CharacterPtr->GetController<APlanetAIController>())
	{
		AICPtr->MoveToActor(
			GameplayAbilityTargetDataSPtr->TriggerCharacterPtr.Get(),
			AcceptanceRadius
		);
		AICPtr->ReceiveMoveCompleted.RemoveDynamic(this, &ThisClass::OnMoveCompleted);
		AICPtr->ReceiveMoveCompleted.AddDynamic(this, &ThisClass::OnMoveCompleted);
	}
}

void UCS_PeriodicStateModify_Charm::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	switch (Result)
	{
	case EPathFollowingResult::Success: 
	{
		//
		if (auto PCPtr = CharacterPtr->GetController<APlanetPlayerController>())
		{
			PCPtr->MoveToLocation(
				FVector::ZeroVector,
				GameplayAbilityTargetDataSPtr->TriggerCharacterPtr.Get(),
				AcceptanceRadius
			);
		}
		else if (auto AICPtr = CharacterPtr->GetController<APlanetAIController>())
		{
			AICPtr->MoveToActor(
				GameplayAbilityTargetDataSPtr->TriggerCharacterPtr.Get(),
				AcceptanceRadius
			);
		}
	}
		break;
	case EPathFollowingResult::Blocked:
		break;
	case EPathFollowingResult::OffPath:
		break;
	case EPathFollowingResult::Aborted:
		break;
	case EPathFollowingResult::Skipped_DEPRECATED:
		break;
	case EPathFollowingResult::Invalid:
		break;
	default:
		break;
	}
}

void UCS_PeriodicStateModify_Charm::OnTaskTick(UAbilityTask_TimerHelper*, float DeltaTime)
{
	CharacterStateInfoSPtr->TotalTime += DeltaTime;

}
