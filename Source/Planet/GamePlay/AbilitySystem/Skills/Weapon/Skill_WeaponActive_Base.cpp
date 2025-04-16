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

bool FGameplayAbilityTargetData_WeaponActive_ActiveParam::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
)
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

bool USkill_WeaponActive_Base::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	if (WaitInput)
	{
	}
	else
	{
		return false;	
	}
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
	WaitInput = true;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// void USkill_WeaponActive_Base::InitalDefaultTags()
// {
// 	Super::InitalDefaultTags();
//
// 	// GetAssetTags(UGameplayTagsLibrary::Skill_CanBeInterrupted_Stagnation);
// 	//
// 	// ActivationBlockedTags.AddTag(UGameplayTagsLibrary::Skill_CanBeInterrupted_Stagnation);
// 	// ActivationBlockedTags.AddTag(UGameplayTagsLibrary::State_Buff_Stagnation);
// 	// ActivationBlockedTags.AddTag(UGameplayTagsLibrary::State_Debuff_Stun);
// 	// ActivationBlockedTags.AddTag(UGameplayTagsLibrary::State_Debuff_Charm);
// 	// ActivationBlockedTags.AddTag(UGameplayTagsLibrary::State_Debuff_Fear);
// 	//
// 	// ActivationOwnedTags.AddTag(UGameplayTagsLibrary::State_ReleasingSkill);
// }

bool USkill_WeaponActive_Base::GetNum(
	int32& Num
) const
{
	return false;
}

bool USkill_WeaponActive_Base::PerformIfContinue()
{
	WaitInput = true;

#if UE_EDITOR || UE_SERVER
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
	)
	{
		if (GetIsContinue() && CanActivateAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()))
		{
			Cast<UPlanetAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo())->
				ReplicatePerformAction_Server(
					GetCurrentAbilitySpecHandle(),
					GetCurrentActivationInfo()
				);

			return true;
		}
	}
	else
	{
	}
#endif
	
	return false;
}

void USkill_WeaponActive_Base::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	WaitInput = false;

	ResetPreviousStageActions();
}

void USkill_WeaponActive_Base::WaitInputTick(
	UAbilityTask_TimerHelper* InWaitInputTaskPtr,
	float Interval,
	float Duration
)
{
}
