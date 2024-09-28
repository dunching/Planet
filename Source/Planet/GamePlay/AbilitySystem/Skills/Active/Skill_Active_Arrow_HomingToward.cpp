
#include "Skill_Active_Arrow_HomingToward.h"

#include "Net/UnrealNetwork.h"

#include "AbilityTask_FlyAway.h"

#include "GameFramework/RootMotionSource.h"

#include "CharacterBase.h"
#include "CharacterStateInfo.h"
#include "StateProcessorComponent.h"
#include "AbilityTask_TimerHelper.h"
#include "AllocationSkills.h"
#include "ProxyProcessComponent.h"
#include "GameplayTagsSubSystem.h"
#include "Skill_WeaponActive_Bow.h"
#include "SceneElement.h"

void USkill_Active_Arrow_HomingToward::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		// 状态信息
		CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
		CharacterStateInfoSPtr->Tag = SkillUnitPtr->GetUnitType();
		CharacterStateInfoSPtr->Duration = Duration;
		CharacterStateInfoSPtr->DefaultIcon = SkillUnitPtr->GetIcon();
		CharacterStateInfoSPtr->DataChanged();

		CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(CharacterStateInfoSPtr);

		//
		{
			auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
			TaskPtr->SetDuration(Duration, 0.1f);
			TaskPtr->DurationDelegate.BindUObject(this, &ThisClass::DurationTick);
			TaskPtr->OnFinished.BindUObject(this, &ThisClass::OnFinished);
			TaskPtr->ReadyForActivation();
		}

		SwitchIsHomingToward(true);
	}
#endif
}

void USkill_Active_Arrow_HomingToward::DurationTick(UAbilityTask_TimerHelper*, float Interval, float InDuration)
{
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (CharacterStateInfoSPtr)
		{
			CharacterStateInfoSPtr->TotalTime = Interval;
			CharacterStateInfoSPtr->DataChanged();
			CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(CharacterStateInfoSPtr);
		}
	}
#endif
}

bool USkill_Active_Arrow_HomingToward::OnFinished(UAbilityTask_TimerHelper*)
{
	SwitchIsHomingToward(false);

	CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(CharacterStateInfoSPtr);

	ThisClass::K2_CancelAbility();

	return true;
}

void USkill_Active_Arrow_HomingToward::SwitchIsHomingToward(bool bIsHomingToward)
{
	// 找到当前装备的 弓箭类武器
	TSharedPtr<FWeaponSkillProxy> FirstWeaponSkillSPtr;
	TSharedPtr<FWeaponSkillProxy> SecondWeaponSkillSPtr;
	CharacterPtr->GetProxyProcessComponent()->GetWeaponSkills(FirstWeaponSkillSPtr, SecondWeaponSkillSPtr);
	if (FirstWeaponSkillSPtr)
	{
		auto RegisterParamPtr =
			Cast<USkill_WeaponActive_Bow>(FirstWeaponSkillSPtr->GetGAInst())->RegisterParamSPtr;
		auto GameplayAbilityTargetPtr =
			DeepClone_GameplayAbilityTargetData<FGameplayAbilityTargetData_Bow_RegisterParam>(RegisterParamPtr.Get());

		GameplayAbilityTargetPtr->bIsHomingTowards = bIsHomingToward;

		FGameplayEventData GameplayEventData;
		GameplayEventData.TargetData.Add(GameplayAbilityTargetPtr);

		Cast<UPlanetAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent)->Replicate_UpdateGAParam(
			FirstWeaponSkillSPtr->GetGAHandle(),
			GameplayEventData
		);
	}
}
