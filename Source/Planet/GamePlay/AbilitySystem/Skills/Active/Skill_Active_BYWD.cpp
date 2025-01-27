
#include "Skill_Active_BYWD.h"

#include "Net/UnrealNetwork.h"

#include "AbilityTask_FlyAway.h"

#include "GameFramework/RootMotionSource.h"

#include "CharacterBase.h"
#include "CharacterStateInfo.h"
#include "StateProcessorComponent.h"
#include "AbilityTask_TimerHelper.h"
#include "AssetRefMap.h"
#include "GameplayTagsLibrary.h"

void USkill_Active_BYWD::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);

#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode()  == NM_DedicatedServer)
	{
		CommitAbility(Handle, ActorInfo, ActivationInfo);
	}
#endif
	
#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode()  == NM_DedicatedServer)
	{
		// CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
		// CharacterStateInfoSPtr->Tag = SkillProxyPtr->GetProxyType();
		// CharacterStateInfoSPtr->Duration = Duration;
		// CharacterStateInfoSPtr->DefaultIcon = SkillProxyPtr->GetIcon();
		// CharacterStateInfoSPtr->DataChanged();
		//
		// CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(CharacterStateInfoSPtr);

		{
			auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
			TaskPtr->SetDuration(Duration, 0.1f);
			TaskPtr->DurationDelegate.BindUObject(this, &ThisClass::DurationDelegate);
			TaskPtr->ReadyForActivation();
		}
	}
#endif

	if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() > ROLE_SimulatedProxy)
	{
		{
			auto TaskPtr = UAbilityTask_FlyAway::NewTask(
				this,
				TEXT(""),
				ERootMotionAccumulateMode::Additive,
				Duration,
				Height,
				Height,
				-1
			);

#if UE_EDITOR || UE_SERVER
			if (GetAbilitySystemComponentFromActorInfo()->GetNetMode()  == NM_DedicatedServer)
			{
				TaskPtr->OnFinished.BindLambda([this]
					{
						CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(CharacterStateInfoSPtr);

						K2_CancelAbility();
					});
			}
#endif

			TaskPtr->ReadyForActivation();
		}
	}
}

bool USkill_Active_BYWD::CommitAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
)
{
	FGameplayEffectSpecHandle SpecHandle =
	   MakeOutgoingGameplayEffectSpec(UAssetRefMap::GetInstance()->DurationGEClass, GetAbilityLevel());

	SpecHandle.Data.Get()->SetDuration(Duration, true);
	SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_Info);
	SpecHandle.Data.Get()->SetSetByCallerMagnitude(UGameplayTagsLibrary::GEData_Duration,1);
				
	const auto GEHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	
	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void USkill_Active_BYWD::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle =
		   MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->AddDynamicAssetTag(SkillProxyPtr->GetProxyType());
				
		const auto CDGEHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
	
	// 公共冷却
}

void USkill_Active_BYWD::DurationDelegate(UAbilityTask_TimerHelper*, float CurrentInterval, float Interval)
{
#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode()  == NM_DedicatedServer)
	{
		if (CharacterStateInfoSPtr)
		{
			CharacterStateInfoSPtr->TotalTime = CurrentInterval;
			CharacterStateInfoSPtr->DataChanged();
			CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(CharacterStateInfoSPtr);
		}
	}
#endif
}
