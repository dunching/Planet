
#include "Skill_Active_Shield.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"
#include "Components/SplineComponent.h"
#include <Components/CapsuleComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "Kismet/KismetMathLibrary.h"
#include <Engine/OverlapResult.h>
#include <GameFramework/SpringArmComponent.h>
#include "Net/UnrealNetwork.h"


#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "Tool_PickAxe.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "Planet.h"
#include "CollisionDataStruct.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "AbilityTask_TimerHelper.h"
#include "Helper_RootMotionSource.h"
#include "AbilityTask_tornado.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAbilitySystemComponent.h"
#include "CameraTrailHelper.h"
#include "AbilityTask_ControlCameraBySpline.h"
#include "AS_Character.h"
#include "CharacterAttibutes.h"
#include "CharacterAttributesComponent.h"
#include "KismetGravityLibrary.h"
#include "StateProcessorComponent.h"
#include "CharacterStateInfo.h"

void USkill_Active_Shield::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);

#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode()  == NM_DedicatedServer)
	{
		CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());

		// 数值修改

		// 持续时间
		auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
		TaskPtr->SetDuration(Duration);
		TaskPtr->DurationDelegate.BindUObject(this, &ThisClass::DurationDelegate);
		TaskPtr->OnFinished.BindLambda([this](auto) {
			K2_CancelAbility();
			return true;
			});
		TaskPtr->ReadyForActivation();
	}
#endif
}

bool USkill_Active_Shield::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	if (PP > CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetPP())
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USkill_Active_Shield::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode()  == NM_DedicatedServer)
	{
		// 清空 数值修改
	}
#endif

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool USkill_Active_Shield::CommitAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
)
{
	// 数值修改
	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void USkill_Active_Shield::DurationDelegate(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval)
{
#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode()  == NM_DedicatedServer)
	{
		if (CharacterStateInfoSPtr)
		{
			CharacterStateInfoSPtr->TotalTime = CurrentInterval;
			CharacterStateInfoSPtr->DataChanged();
		}
	}
#endif
}
