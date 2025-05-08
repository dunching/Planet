
#include "Skill_Active_Arrow_Multiple.h"

#include "Net/UnrealNetwork.h"

#include "AbilityTask_ApplyRootMotion_FlyAway.h"

#include "GameFramework/RootMotionSource.h"

#include "CharacterBase.h"
#include "CharacterStateInfo.h"
#include "StateProcessorComponent.h"
#include "AbilityTask_TimerHelper.h"
#include "AllocationSkills.h"
#include "ProxyProcessComponent.h"
#include "GameplayTagsLibrary.h"
#include "Skill_WeaponActive_Bow.h"
#include "ItemProxy_Minimal.h"
#include "SceneProxyTable.h"

void USkill_Active_Arrow_Multiple::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
	)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (SkillProxyPtr)
	{
		ItemProxy_DescriptionPtr = Cast<FItemProxy_DescriptionType>(
			DynamicCastSharedPtr<FActiveSkillProxy>(SkillProxyPtr)->GetTableRowProxy_ActiveSkillExtendInfo()
		);
	}
}

void USkill_Active_Arrow_Multiple::ApplyCooldown(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo
	) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->AddDynamicAssetTag(SkillProxyPtr->GetProxyType());
		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_CD);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			UGameplayTagsLibrary::GEData_Duration,
			ItemProxy_DescriptionPtr->CD.PerLevelValue[0]
		);

		const auto CDGEHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

void USkill_Active_Arrow_Multiple::ApplyCost(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo
	) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->AddDynamicAssetTag(SkillProxyPtr->GetProxyType());
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			UGameplayTagsLibrary::GEData_ModifyItem_Mana,
			ItemProxy_DescriptionPtr->Cost.PerLevelValue[0]
		);

		const auto CDGEHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

void USkill_Active_Arrow_Multiple::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);

#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
	{
		SwitchIsMultiple(true);

		CommitAbility(Handle, ActorInfo, ActivationInfo);
	}
#endif

	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
	)
	{
		// 状态信息


		//
		{
			auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
			TaskPtr->SetDuration(Duration, 0.1f);
			TaskPtr->DurationDelegate.BindUObject(this, &ThisClass::OnDuration);
			if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
			{
				TaskPtr->OnFinished.BindUObject(this, &ThisClass::OnFinished);
			}
			TaskPtr->ReadyForActivation();
		}
	}
}

void USkill_Active_Arrow_Multiple::OnDuration(
	UAbilityTask_TimerHelper*,
	float CurrentTiem,
	float TotalTime
	)
{
	if (FMath::IsNearlyZero(TotalTime))
	{
		return;
	}
	
	RemainTime = (TotalTime - CurrentTiem) / TotalTime;
	if (RemainTime < 0.f)
	{
		RemainTime = 0.f;
	}
}

bool USkill_Active_Arrow_Multiple::OnFinished(UAbilityTask_TimerHelper*)
{
	SwitchIsMultiple(false);


	K2_CancelAbility();

	return true;
}

void USkill_Active_Arrow_Multiple::SwitchIsMultiple(bool bIsMultiple)
{
	auto TargetSkillSPtr = CharacterPtr->GetProxyProcessComponent()->GetWeaponSkillByType(
		UGameplayTagsLibrary::Proxy_Skill_Weapon_Bow
	);

	if (TargetSkillSPtr)
	{
		auto RegisterParamPtr =
			Cast<USkill_WeaponActive_Bow>(TargetSkillSPtr->GetGAInst())->RegisterParamSPtr;
		auto GameplayAbilityTargetPtr =
			DeepClone_GameplayAbilityTargetData<FGameplayAbilityTargetData_Bow_RegisterParam>(RegisterParamPtr.Get());

		GameplayAbilityTargetPtr->bIsMultiple = bIsMultiple;

		FGameplayEventData GameplayEventData;
		GameplayEventData.TargetData.Add(GameplayAbilityTargetPtr);

		Cast<UPlanetAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent)->Replicate_UpdateGAParam(
			TargetSkillSPtr->GetGAHandle(),
			GameplayEventData
		);
	}
}

float USkill_Active_Arrow_Multiple::GetRemainTime() const
{
	return RemainTime;
}
