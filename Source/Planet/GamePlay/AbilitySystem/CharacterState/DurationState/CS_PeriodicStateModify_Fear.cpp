
#include "CS_PeriodicStateModify_Fear.h"

#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>
#include "AbilitySystemGlobals.h"
#include <GameFramework/CharacterMovementComponent.h>

#include "KismetGravityLibrary.h"
#include "NavigationSystem.h"

#include "AbilityTask_TimerHelper.h"
#include "CharacterBase.h"
#include "UnitProxyProcessComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "BaseFeatureGAComponent.h"
#include "GameplayTagsSubSystem.h"
#include "AbilityTask_MyApplyRootMotionConstantForce.h"
#include "AbilityTask_FlyAway.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "StateProcessorComponent.h"
#include "CharacterStateInfo.h"
#include "PlanetPlayerController.h"
#include "CharacterAttibutes.h"

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
	Super(UGameplayTagsSubSystem::GetInstance()->State_Debuff_Fear, Duration)
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

	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantPlayerInputMove);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantJump);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantRootMotion);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantRotation);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_Orient2Acce);
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

	//
	CharacterPtr->GetCharacterAttributesComponent()->CharacterAttributes.MoveSpeed.RemoveSettlementModify(MyPropertySettlementModify);

	//
	auto PCPtr = CharacterPtr->GetController<APlanetPlayerController>();
	PCPtr->ReceiveMoveCompleted.Unbind();
	PCPtr->StopMovement_RPC();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCS_PeriodicStateModify_Fear::UpdateDuration()
{
	Super::UpdateDuration();

	if (TaskPtr)
	{
		TaskPtr->SetDuration(GameplayAbilityTargetDataSPtr->Duration);
		TaskPtr->UpdateDuration();
	}

	PerformAction();
}

void UCS_PeriodicStateModify_Fear::PerformAction()
{
	// 
	CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
	CharacterStateInfoSPtr->Tag = GameplayAbilityTargetDataSPtr->Tag;
	CharacterStateInfoSPtr->Duration = GameplayAbilityTargetDataSPtr->Duration;
	CharacterStateInfoSPtr->DefaultIcon = GameplayAbilityTargetDataSPtr->DefaultIcon;
	CharacterStateInfoSPtr->DataChanged();
	CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(CharacterStateInfoSPtr);

	//
	MyPropertySettlementModify = MakeShared<FMyPropertySettlementModify>();
	CharacterPtr->GetCharacterAttributesComponent()->CharacterAttributes.MoveSpeed.AddSettlementModify(MyPropertySettlementModify);

	//
	MoveImp();
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
		auto PCPtr = CharacterPtr->GetController<APlanetPlayerController>();
		PCPtr->MoveToLocation_RPC(
			TargetPt,
			nullptr,
			AcceptanceRadius
		);
		PCPtr->ReceiveMoveCompleted.BindUObject(this, &ThisClass::OnMoveCompleted);
	}
}
