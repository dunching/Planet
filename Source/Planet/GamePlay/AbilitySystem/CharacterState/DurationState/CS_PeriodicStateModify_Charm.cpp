
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
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "BaseFeatureComponent.h"
#include "GameplayTagsSubSystem.h"
#include "AbilityTask_MyApplyRootMotionConstantForce.h"
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
	Super(UGameplayTagsSubSystem::GetInstance()->State_Debuff_Charm, Duration)
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
	CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(CharacterStateInfoSPtr);

	//
	auto GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent;

	GAEventDataPtr->TriggerCharacterPtr = GameplayAbilityTargetDataSPtr->TargetCharacterPtr;

	FGAEventData GAEventData(
		GameplayAbilityTargetDataSPtr->TargetCharacterPtr,
		GameplayAbilityTargetDataSPtr->TargetCharacterPtr
	);

	GAEventData.DataSource = GameplayAbilityTargetDataSPtr->Tag;
	GAEventData.DataModify = { {ECharacterPropertyType::MoveSpeed,0 } };
	GAEventData.bIsClearData = true;

	GAEventDataPtr->DataAry.Add(GAEventData);

	CharacterPtr->GetBaseFeatureComponent()->SendEventImp(GAEventDataPtr);

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

void UCS_PeriodicStateModify_Charm::InitalDefaultTags()
{
	Super::InitalDefaultTags();

	AbilityTags.AddTag(UGameplayTagsSubSystem::GetInstance()->State_Debuff_Charm);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->State_Debuff_Charm);

	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantPlayerInputMove);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantJump);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantRootMotion);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantRotation);;
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_Orient2Acce);
}

void UCS_PeriodicStateModify_Charm::PerformAction()
{
	// 
	CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
	CharacterStateInfoSPtr->Tag = GameplayAbilityTargetDataSPtr->Tag;
	CharacterStateInfoSPtr->Duration = GameplayAbilityTargetDataSPtr->Duration;
	CharacterStateInfoSPtr->DefaultIcon = GameplayAbilityTargetDataSPtr->DefaultIcon;
	CharacterStateInfoSPtr->DataChanged();
	CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(CharacterStateInfoSPtr);

	//
	auto GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent;

	GAEventDataPtr->TriggerCharacterPtr = GameplayAbilityTargetDataSPtr->TargetCharacterPtr;

	FGAEventData GAEventData(
		GameplayAbilityTargetDataSPtr->TargetCharacterPtr,
		GameplayAbilityTargetDataSPtr->TargetCharacterPtr
	);

	GAEventData.DataSource = GameplayAbilityTargetDataSPtr->Tag;
	GAEventData.DataModify = { {ECharacterPropertyType::MoveSpeed,0 } };
	GAEventData.bIsOverlapData = true;

	GAEventDataPtr->DataAry.Add(GAEventData);

	CharacterPtr->GetBaseFeatureComponent()->SendEventImp(GAEventDataPtr);

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

	CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(CharacterStateInfoSPtr);
}
