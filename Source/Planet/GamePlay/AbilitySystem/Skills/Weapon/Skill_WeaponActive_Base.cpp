
#include "Skill_WeaponActive_Base.h"

#include "CharacterBase.h"
#include "AbilityTask_TimerHelper.h"
#include "LogWriter.h"
#include "Weapon_Base.h"

UScriptStruct* FGameplayAbilityTargetData_Skill_Weapon::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_Skill_Weapon::StaticStruct();
}

bool FGameplayAbilityTargetData_Skill_Weapon::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << bIsAutoContinue;
	Ar << WeaponPtr;

	return true;
}

USkill_WeaponActive_Base::USkill_WeaponActive_Base() :
	Super()
{
//	bRetriggerInstancedAbility = true;
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

	WaitInputTaskPtr = nullptr;

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		ActiveParamPtr = dynamic_cast<const ActiveParamType*>(TriggerEventData->TargetData.Get(0));
		if (ActiveParamPtr)
		{
		}
		else
		{
			return;
		}
	}
}

void USkill_WeaponActive_Base::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool USkill_WeaponActive_Base::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USkill_WeaponActive_Base::CancelAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility
)
{
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
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_WeaponActive_Base::SetContinuePerformImp(bool bIsContinue_)
{
	if (bIsContinue_)
	{
		ContinueActive();
	}
	else
	{
		StopContinueActive();
	}
}

void USkill_WeaponActive_Base::ContinueActive()
{
	if (!CanActivateAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()))
	{
		return;
	}

	bIsContinue = true;
	if (WaitInputTaskPtr)
	{
		WaitInputTaskPtr->ExternalCancel();
		WaitInputTaskPtr = nullptr;

		PerformAction(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), &CurrentEventData);
	}
	else
	{
	}
}

void USkill_WeaponActive_Base::StopContinueActive()
{
	bIsContinue = false;
}

void USkill_WeaponActive_Base::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	ResetPreviousStageActions();
	bIsContinue = true;
}

void USkill_WeaponActive_Base::CheckInContinue()
{
	if (bIsContinue)
	{
		PerformAction(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), &CurrentEventData);
	}
	else
	{
		WaitInputPercent = 1.f;

		WaitInputTaskPtr = UAbilityTask_TimerHelper::DelayTask(this);

		if (CurrentWaitInputTime > 0.f)
		{
			WaitInputTaskPtr->SetDuration(CurrentWaitInputTime, 0.1f);
			WaitInputTaskPtr->DurationDelegate.BindUObject(this, &ThisClass::WaitInputTick);
			WaitInputTaskPtr->OnFinished.BindLambda([this](auto) {
				K2_CancelAbility();
				return true;
				});
		}
		else
		{
			WaitInputTaskPtr->SetInfinite();
		}

		WaitInputTaskPtr->ReadyForActivation();
	}
}

void USkill_WeaponActive_Base::WaitInputTick(UAbilityTask_TimerHelper* InWaitInputTaskPtr, float Interval, float Duration)
{
	if (Duration > 0.f)
	{
		WaitInputPercent = FMath::Clamp(1.f - (Interval / Duration), 0.f, 1.f);
	}
	else
	{
		check(0);
		WaitInputPercent = 1.f;
	}
}