
#include "CS_PeriodicStateModify_Fear.h"

#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>
#include "AbilitySystemGlobals.h"
#include <GameFramework/CharacterMovementComponent.h>

#include "KismetGravityLibrary.h"
#include "NavigationSystem.h"

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
#include "GameplayTagsLibrary.h"
#include "AbilityTask_ARM_ConstantForce.h"
#include "AbilityTask_FlyAway.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "StateProcessorComponent.h"
#include "CharacterStateInfo.h"
#include "PlanetPlayerController.h"
#include "CharacterAttibutes.h"
#include "PlanetAIController.h"

struct FMyPropertySettlementModify : public FPropertySettlementModify
{
	int32 MoveSpeed = 200;

	FMyPropertySettlementModify() :
		FPropertySettlementModify(10)
	{

	}

	virtual int32 SettlementModify(const TMap<FGameplayTag, int32>& ValueMap)const override
	{
		const auto Result = FPropertySettlementModify::SettlementModify(ValueMap);
		return MoveSpeed < Result ? MoveSpeed : Result;
	}
};

FGameplayAbilityTargetData_StateModify_Fear::FGameplayAbilityTargetData_StateModify_Fear(
	float Duration
) :
	Super(UGameplayTagsLibrary::State_Debuff_Fear, Duration)
{
}

FGameplayAbilityTargetData_StateModify_Fear::FGameplayAbilityTargetData_StateModify_Fear()
{

}

void UCS_PeriodicStateModify_Fear::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void UCS_PeriodicStateModify_Fear::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UCS_PeriodicStateModify_Fear::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UCS_PeriodicStateModify_Fear::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	//
	CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(CharacterStateInfoSPtr);
	CharacterStateInfoSPtr = nullptr;

	//
	// CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().MoveSpeed.RemoveSettlementModify(MyPropertySettlementModify);

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

void UCS_PeriodicStateModify_Fear::UpdateDuration()
{
	Super::UpdateDuration();

	if (TimerTaskPtr)
	{
		TimerTaskPtr->SetDuration(GameplayAbilityTargetDataSPtr->Duration);
		TimerTaskPtr->UpdateDuration();
	}

	PerformAction();
}

void UCS_PeriodicStateModify_Fear::PerformAction()
{
	if (CharacterStateInfoSPtr)
	{
		CharacterStateInfoSPtr->Duration = GameplayAbilityTargetDataSPtr->Duration;
		CharacterStateInfoSPtr->RefreshTime();
	}
	else
	{
		// 
		CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
		CharacterStateInfoSPtr->Tag = GameplayAbilityTargetDataSPtr->Tag;
		CharacterStateInfoSPtr->Duration = GameplayAbilityTargetDataSPtr->Duration;
		CharacterStateInfoSPtr->DefaultIcon = GameplayAbilityTargetDataSPtr->DefaultIcon;
		CharacterStateInfoSPtr->DataChanged();
		CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(CharacterStateInfoSPtr);

		// TalentHelper.Level > 

		//
		MoveImp();
	}
}

void UCS_PeriodicStateModify_Fear::InitalDefaultTags()
{
	Super::InitalDefaultTags();

	AbilityTags.AddTag(UGameplayTagsLibrary::State_Debuff_Fear);
	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::State_Debuff_Fear);

	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantPlayerInputMove);
	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantJump);
	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantRootMotion);
	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantRotation);
	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_Orient2Acce);
}

void UCS_PeriodicStateModify_Fear::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	switch (Result)
	{
	case EPathFollowingResult::Success:
	{
		//
		MoveImp();
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

void UCS_PeriodicStateModify_Fear::OnTaskTick(UAbilityTask_TimerHelper*, float DeltaTime)
{
	CharacterStateInfoSPtr->TotalTime += DeltaTime;

	CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(CharacterStateInfoSPtr);
}

void UCS_PeriodicStateModify_Fear::MoveImp()
{
	FVector TargetPt = FVector::ZeroVector;
	if (UNavigationSystemV1::K2_GetRandomReachablePointInRadius(
		CharacterPtr,
		CharacterPtr->GetActorLocation(),
		TargetPt,
		800.f
	))
	{
		if (auto PCPtr = CharacterPtr->GetController<APlanetPlayerController>())
		{
			PCPtr->MoveToLocation(
				TargetPt,
				nullptr,
				AcceptanceRadius
			);
			PCPtr->ReceiveMoveCompleted.BindUObject(this, &ThisClass::OnMoveCompleted);
		}
		else if (auto AICPtr = CharacterPtr->GetController<APlanetAIController>())
		{
			AICPtr->MoveToLocation(
				TargetPt,
				AcceptanceRadius
			);
			AICPtr->ReceiveMoveCompleted.RemoveDynamic(this, &ThisClass::OnMoveCompleted);
			AICPtr->ReceiveMoveCompleted.AddDynamic(this, &ThisClass::OnMoveCompleted);
		}
	}
}
