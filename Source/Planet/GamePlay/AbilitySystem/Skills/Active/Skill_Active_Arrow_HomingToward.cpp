
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
#include "ItemProxy.h"

void USkill_Active_Arrow_HomingToward::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);

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

		CommitAbility(Handle, ActorInfo, ActivationInfo);
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

	K2_CancelAbility();

	return true;
}

void USkill_Active_Arrow_HomingToward::SwitchIsHomingToward(bool bIsHomingToward)
{
	auto TargetSkillSPtr = CharacterPtr->GetProxyProcessComponent()->GetWeaponSkillByType(
		UGameplayTagsSubSystem::Unit_Skill_Weapon_Bow
	);

	if (TargetSkillSPtr)
	{
		auto RegisterParamPtr =
			Cast<USkill_WeaponActive_Bow>(TargetSkillSPtr->GetGAInst())->RegisterParamSPtr;
		auto GameplayAbilityTargetPtr =
			DeepClone_GameplayAbilityTargetData<FGameplayAbilityTargetData_Bow_RegisterParam>(RegisterParamPtr.Get());

		GameplayAbilityTargetPtr->bIsHomingTowards = bIsHomingToward;

		FGameplayEventData GameplayEventData;
		GameplayEventData.TargetData.Add(GameplayAbilityTargetPtr);

		Cast<UPlanetAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent)->Replicate_UpdateGAParam(
			TargetSkillSPtr->GetGAHandle(),
			GameplayEventData
		);
	}
}
