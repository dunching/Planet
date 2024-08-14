
#include "Skill_WeaponActive_Base.h"

#include "CharacterBase.h"
#include "LogWriter.h"

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
	switch (SkillState)
	{
	case EType::kRunning:
	{
		return false;
	}
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

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());

		auto GameplayAbilityTargetDataPtr = dynamic_cast<const FGameplayAbilityTargetData_Skill_Weapon*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetDataPtr)
		{
			bIsAutomaticStop = GameplayAbilityTargetDataPtr->bIsAutomaticStop;
		}
		else
		{
			return;
		}
	}

	ResetPreviousStageActions();

	SkillState = EType::kNone;

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
	ResetListLock();

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
	SkillState = EType::kFinished;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_WeaponActive_Base::ForceCancel()
{
	bIsRequstCancel = true;
	DecrementToZeroListLock();
}

void USkill_WeaponActive_Base::RequestCancel()
{
	bIsRequstCancel = true;
}

void USkill_WeaponActive_Base::ContinueActive()
{
	if (!CanActivateAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()))
	{
		return;
	}

	bIsRequstCancel = false;

	switch (SkillState)
	{
	case USkill_WeaponActive_Base::EType::kAttackingEnd:
	{
		RepeatAction();
	}
	break;
	default:
		break;
	}
}

void USkill_WeaponActive_Base::PerformAction()
{
	SkillState = EType::kRunning;
}

void USkill_WeaponActive_Base::PerformStopAction()
{
}

bool USkill_WeaponActive_Base::IsEnd() const
{
	switch (SkillState)
	{
	case EType::kAttackingEnd:
	case EType::kFinished:
	{
		return true;
	}
	}
	return false;
}

void USkill_WeaponActive_Base::RepeatAction()
{
	// 运行之这里，说明 ScopeLockCount == 0

	if (bIsRequstCancel)
	{
		PRINTINVOKEINFO();
		ResetPreviousStageActions();

		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ThisClass::K2_CancelAbility));

		PerformStopAction();

		RunIfListLock();
	}
	else
	{
		// AI自动释放技能时停止方式
		if (bIsAutomaticStop && IsEnd())
		{
			PRINTINVOKEINFO();
			K2_CancelAbility();
			return;
		}

		if (CanActivateAbility(CurrentSpecHandle, CurrentActorInfo))
		{
			PRINTINVOKEINFO();
			ResetPreviousStageActions();

			WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ThisClass::RepeatAction));

			PerformAction();

			RunIfListLock();
		}
		// 被其他技能打断
		else
		{
			PRINTINVOKEINFO();
			K2_CancelAbility();
		}
	}
}
