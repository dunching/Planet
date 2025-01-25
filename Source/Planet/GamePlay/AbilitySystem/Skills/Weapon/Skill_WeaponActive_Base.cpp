#include "Skill_WeaponActive_Base.h"

#include "CharacterBase.h"
#include "AbilityTask_TimerHelper.h"
#include "LogWriter.h"
#include "Weapon_Base.h"
#include "GameplayTagsLibrary.h"

UScriptStruct* FGameplayAbilityTargetData_WeaponActive_ActiveParam::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_WeaponActive_ActiveParam::StaticStruct();
}

bool FGameplayAbilityTargetData_WeaponActive_ActiveParam::NetSerialize(FArchive& Ar, class UPackageMap* Map,
                                                                       bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << bIsAutoContinue;
	Ar << WeaponPtr;

	return true;
}

FGameplayAbilityTargetData_WeaponActive_ActiveParam* FGameplayAbilityTargetData_WeaponActive_ActiveParam::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_WeaponActive_ActiveParam;

	*ResultPtr = *this;

	return ResultPtr;
}

USkill_WeaponActive_Base::USkill_WeaponActive_Base() :
	Super()
{
	//	bRetriggerInstancedAbility = true;
}

void USkill_WeaponActive_Base::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);
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
		ActiveParamSPtr = MakeSPtr_GameplayAbilityTargetData<FActiveParamType>(TriggerEventData->TargetData.Get(0));
		if (ActiveParamSPtr)
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

void USkill_WeaponActive_Base::SetContinuePerform(bool bIsContinue_)
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

void USkill_WeaponActive_Base::InitalDefaultTags()
{
	Super::InitalDefaultTags();

	AbilityTags.AddTag(UGameplayTagsLibrary::Skill_CanBeInterrupted_Stagnation);

	ActivationBlockedTags.AddTag(UGameplayTagsLibrary::Skill_CanBeInterrupted_Stagnation);
	ActivationBlockedTags.AddTag(UGameplayTagsLibrary::State_Buff_Stagnation);
	ActivationBlockedTags.AddTag(UGameplayTagsLibrary::State_Debuff_Stun);
	ActivationBlockedTags.AddTag(UGameplayTagsLibrary::State_Debuff_Charm);
	ActivationBlockedTags.AddTag(UGameplayTagsLibrary::State_Debuff_Fear);

	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::State_ReleasingSkill);
}

bool USkill_WeaponActive_Base::GetNum(int32& Num) const
{
	return false;
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

		PerformAction(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(),
		              &CurrentEventData);
	}
	else
	{
	}
}

void USkill_WeaponActive_Base::StopContinueActive()
{
	bIsContinue = false;
}

void USkill_WeaponActive_Base::CheckInContinue(float InWaitInputTime)
{
	if (bIsContinue && CanActivateAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()))
	{
		Cast<UPlanetAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo())->ReplicatePerformAction_Server(
			GetCurrentAbilitySpecHandle(),
			GetCurrentActivationInfo()
		);
	}
	else
	{
		WaitInputPercent = 1.f;

		WaitInputTaskPtr = UAbilityTask_TimerHelper::DelayTask(this);

		if (InWaitInputTime > 0.f)
		{
			WaitInputTaskPtr->SetDuration(InWaitInputTime, 0.1f);
			
#if UE_EDITOR || UE_SERVER
			if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
			{
				WaitInputTaskPtr->DurationDelegate.BindUObject(this, &ThisClass::WaitInputTick);
			}
#endif
			
#if UE_EDITOR || UE_SERVER
			if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
			{
				WaitInputTaskPtr->OnFinished.BindLambda([this](auto)
				{
					K2_CancelAbility();
					return true;
				});
			}
#endif
		}
		else
		{
			WaitInputTaskPtr->SetInfinite();
		}

		WaitInputTaskPtr->ReadyForActivation();
	}
}

void USkill_WeaponActive_Base::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ResetPreviousStageActions();
	bIsContinue = true;
}

void USkill_WeaponActive_Base::WaitInputTick(UAbilityTask_TimerHelper* InWaitInputTaskPtr, float Interval,
                                             float Duration)
{
	if (Duration > 0.f)
	{
		WaitInputPercent = FMath::Clamp(1.f - (Interval / Duration), 0.f, 1.f);
	}
	else
	{
		checkNoEntry();
		WaitInputPercent = 1.f;
	}
}
