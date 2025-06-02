
#include "Skill_Active_Base.h"

#include "Net/UnrealNetwork.h"

#include "ItemProxy_Minimal.h"
#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "AbilityTask_TimerHelper.h"
#include "PlanetWorldSettings.h"
#include "PlanetPlayerController.h"
#include "Tools.h"
#include "SceneProxyTable.h"
#include "GameplayTagsLibrary.h"
#include "GroupManagger.h"
#include "ItemProxy_Skills.h"
#include "ItemProxy_Weapon.h"
#include "SceneProxyExtendInfo.h"

UScriptStruct* FGameplayAbilityTargetData_ActiveSkill_ActiveParam::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_ActiveSkill_ActiveParam::StaticStruct();
}

bool FGameplayAbilityTargetData_ActiveSkill_ActiveParam::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << bIsAutoContinue;

	return true;
}

FGameplayAbilityTargetData_ActiveSkill_ActiveParam* FGameplayAbilityTargetData_ActiveSkill_ActiveParam::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_ActiveSkill_ActiveParam;

	*ResultPtr = *this;

	return ResultPtr;
}

USkill_Active_Base::USkill_Active_Base():
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USkill_Active_Base::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (SkillProxyPtr)
	{
		DynamicCastSharedPtr<FActiveSkillProxy>(SkillProxyPtr)->OffsetCooldownTime();
	}

	if (SkillProxyPtr)
	{
		ItemProxy_DescriptionPtr = Cast<FItemProxy_DescriptionType>(
			DynamicCastSharedPtr<FActiveSkillProxy>(SkillProxyPtr)->GetTableRowProxy_ActiveSkillExtendInfo()
		);
	}
}

void USkill_Active_Base::PreActivate(
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
		ActiveParamPtr = dynamic_cast<const FGameplayAbilityTargetData_ActiveSkill_ActiveParam*>(TriggerEventData->TargetData.Get(0));
		if (ActiveParamPtr)
		{
			bIsPreviouInput = ActiveParamPtr->bIsAutoContinue;
		}
	}
}

void USkill_Active_Base::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool USkill_Active_Base::CommitAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
)
{
	DynamicCastSharedPtr<FActiveSkillProxy>(SkillProxyPtr)->ApplyCooldown();

	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

bool USkill_Active_Base::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	auto ActiveSkillProxyPtr = DynamicCastSharedPtr<FActiveSkillProxy>(SkillProxyPtr);
	if (!ActiveSkillProxyPtr)
	{
		return false;
	}
	if (!ActiveSkillProxyPtr->CheckNotInCooldown())
	{
		return false;
	}

	const auto RequireWeaponProxyType = ActiveSkillProxyPtr->GetTableRowProxy_ActiveSkillExtendInfo()->RequireWeaponProxyType;

	TSharedPtr<FWeaponProxy>FirstWeaponProxySPtr = nullptr;
	TSharedPtr<FWeaponProxy>SecondWeaponProxySPtr = nullptr;
	CharacterPtr->GetProxyProcessComponent()->GetWeaponProxy(
		FirstWeaponProxySPtr,
		SecondWeaponProxySPtr
	);

	if (
		FirstWeaponProxySPtr &&
		(FirstWeaponProxySPtr->GetProxyType() == RequireWeaponProxyType)
		)
	{
	}
	else if (
		SecondWeaponProxySPtr &&
		(SecondWeaponProxySPtr->GetProxyType() == RequireWeaponProxyType)
		)
	{
	}
	else
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USkill_Active_Base::CancelAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility
)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void USkill_Active_Base::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	WaitInputTaskPtr = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// void USkill_Active_Base::InitalDefaultTags()
// {
// 	Super::InitalDefaultTags();
//
// 	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::State_ReleasingSkill);
//
// 	ActivationBlockedTags.AddTag(UGameplayTagsLibrary::Skill_CanBeInterrupted_Stagnation);
// 	ActivationBlockedTags.AddTag(UGameplayTagsLibrary::State_Buff_Stagnation);
// }

void USkill_Active_Base::GetInputRemainPercent(bool& bIsAcceptInput, float& Percent) const
{
	if (WaitInputTaskPtr)
	{
		bIsAcceptInput = true;
		Percent = WaitInputPercent;
	}
	else
	{
		bIsAcceptInput = false;
		Percent = 0.f;
	}
}

void USkill_Active_Base::CheckInContinue()
{
	if (bIsPreviouInput || ActiveParamPtr->bIsAutoContinue)
	{
		PerformAction(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), &CurrentEventData);
		bIsPreviouInput = false;
	}
	else
	{
		WaitInputPercent = 1.f;

		WaitInputTaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
		WaitInputTaskPtr->SetDuration(CurrentWaitInputTime, 0.1f);
		WaitInputTaskPtr->DurationDelegate.BindUObject(this, &ThisClass::WaitInputTick);
		WaitInputTaskPtr->OnFinished.BindLambda([this](auto) {
			K2_CancelAbility();
			return true;
			});
		WaitInputTaskPtr->ReadyForActivation();
	}
}

void USkill_Active_Base::ContinueActive()
{
	if (!CanActivateAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()))
	{
		return;
	}

	if (WaitInputTaskPtr)
	{
		WaitInputTaskPtr->ExternalCancel();
		WaitInputTaskPtr = nullptr;

		PerformAction(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), &CurrentEventData);
	}
	else
	{
		bIsPreviouInput = true;
	}
}

void USkill_Active_Base::WaitInputTick(UAbilityTask_TimerHelper*, float Interval, float Duration)
{
	if (Duration > 0.f)
	{
		WaitInputPercent =  FMath::Clamp(1.f - (Interval / Duration), 0.f, 1.f);
	}
	else
	{
		checkNoEntry();
		WaitInputPercent = 1.f;
	}
}

void USkill_Active_Base::Tick(float DeltaTime)
{
}

void USkill_Active_Base::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void USkill_Active_Base::ApplyCooldown(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo
	) const
{
	Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);
	
	// 公共冷却
	{
		UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
		
		auto AbilitySystemComponentPtr = CharacterPtr->GetGroupManagger()->GetAbilitySystemComponent();
		const auto SkillCommonCooldownInfoMap = ItemProxy_DescriptionPtr->SkillCommonCooldownInfoMap;
		for (const auto Iter : SkillCommonCooldownInfoMap)
		{
			auto CommonCooldownInfoPtr = GetTableRowProxy_CommonCooldownInfo(Iter);
			if (CommonCooldownInfoPtr)
			{
				FGameplayEffectSpecHandle SpecHandle =
					AbilitySystemComponentPtr->MakeOutgoingSpec(CooldownGE->GetClass(), GetAbilityLevel(),
																AbilitySystemComponentPtr->MakeEffectContext());

				const auto CD = CommonCooldownInfoPtr->CoolDownTime;
				SpecHandle.Data.Get()->SetDuration(CD, true);
				SpecHandle.Data.Get()->AddDynamicAssetTag(Iter);
				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_CD);
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(UGameplayTagsLibrary::GEData_Duration, CD);
				AbilitySystemComponentPtr->ApplyGameplayEffectSpecToSelf(
					*SpecHandle.Data.Get(),
					AbilitySystemComponentPtr->GetPredictionKeyForNewAction()
				);
			}
		}
	}
}

void USkill_Active_Base::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
