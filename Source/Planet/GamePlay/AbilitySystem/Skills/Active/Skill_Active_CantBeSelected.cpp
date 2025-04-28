
#include "Skill_Active_CantBeSelected.h"

#include <Engine/OverlapResult.h>

#include "CharacterBase.h"
#include "AbilityTask_TimerHelper.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAbilitySystemComponent.h"
#include "CameraTrailHelper.h"
#include "AbilityTask_ControlCameraBySpline.h"
#include "CharacterAttibutes.h"
#include "CharacterStateInfo.h"
#include "StateProcessorComponent.h"

void USkill_Active_CantBeSelected::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void USkill_Active_CantBeSelected::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode()  == NM_DedicatedServer)
	{
		CommitAbility(Handle, ActorInfo, ActivationInfo);
		ExcuteTasks();



		{
			auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
			TaskPtr->SetDuration(Duration, 0.1f);
			TaskPtr->DurationDelegate.BindUObject(this, &ThisClass::DurationDelegate);
			TaskPtr->OnFinished.BindLambda([this](auto)
				{
					K2_CancelAbility();
					return true;
				});
			TaskPtr->ReadyForActivation();
		}
	}
#endif
}

void USkill_Active_CantBeSelected::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Active_CantBeSelected::ExcuteTasks()
{
	if (CharacterPtr)
	{
	}
}

void USkill_Active_CantBeSelected::DurationDelegate(UAbilityTask_TimerHelper*, float CurrentIntervalTime, float IntervalTime)
{
#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode()  == NM_DedicatedServer)
	{
		if (CharacterStateInfoSPtr)
		{
		}
	}
#endif
}
