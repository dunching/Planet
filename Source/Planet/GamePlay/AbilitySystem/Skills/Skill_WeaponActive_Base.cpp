
#include "Skill_WeaponActive_Base.h"

USkill_WeaponActive_Base::USkill_WeaponActive_Base() :
	Super()
{
	bRetriggerInstancedAbility = true;
}

bool USkill_WeaponActive_Base::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{

	if (!bIsAttackEnd)
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USkill_WeaponActive_Base::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	ResetPreviousStageActions();

	bIsAttackEnd = false;

	bIsRequstCancel = false;
}

void USkill_WeaponActive_Base::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	RepeatAction();
}

void USkill_WeaponActive_Base::CancelAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility
)
{
	bIsRequstCancel = true;

	// 
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void USkill_WeaponActive_Base::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	bIsAttackEnd = true;

	if (bIsRequstCancel)
	{
		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	}
	else
	{
		PerformAction();
	}
}

void USkill_WeaponActive_Base::ForceCancel()
{
	bIsRequstCancel = true;
	DecrementToZeroListLock();
}

void USkill_WeaponActive_Base::PerformAction()
{
}

void USkill_WeaponActive_Base::PerformStopAction()
{

}

void USkill_WeaponActive_Base::RepeatAction()
{
	if (bIsRequstCancel)
	{
		PerformStopAction();
	}
	else
	{
		ResetPreviousStageActions();

		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ThisClass::RepeatAction));

		PerformAction();
	}
}
