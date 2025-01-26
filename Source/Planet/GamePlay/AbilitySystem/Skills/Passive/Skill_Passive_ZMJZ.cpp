#include "Skill_Passive_ZMJZ.h"

#include "AbilitySystemComponent.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "AbilityTask_TimerHelper.h"
#include "CharacterAbilitySystemComponent.h"
#include "StateProcessorComponent.h"
#include "CS_PeriodicPropertyModify.h"
#include "GAEvent_Helper.h"
#include "GameplayTagsLibrary.h"
#include "GE_Common.h"

void USkill_Passive_ZMJZ::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

#if UE_EDITOR || UE_SERVER
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
	)
	{
		CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
		if (CharacterPtr)
		{
			AbilityActivatedCallbacksHandle =
				CharacterPtr->GetCharacterAbilitySystemComponent()->MakedDamageDelegate.AddCallback(
					std::bind(&ThisClass::OnSendAttack, this, std::placeholders::_1));
		}
	}
#endif
}

void USkill_Passive_ZMJZ::PreActivate(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                      FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
                                      const FGameplayEventData* TriggerEventData /*= nullptr */)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void USkill_Passive_ZMJZ::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void USkill_Passive_ZMJZ::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	if (AbilityActivatedCallbacksHandle)
	{
		AbilityActivatedCallbacksHandle->UnBindCallback();
	}

	Super::OnRemoveAbility(ActorInfo, Spec);
}

void USkill_Passive_ZMJZ::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	const auto ProxyType = SkillProxyPtr->GetProxyType();
	ModifyCharacterData(ProxyType, true);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Passive_ZMJZ::PerformAction(
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
		if (CharacterPtr)
		{
			FGameplayEffectSpecHandle SpecHandle =
				MakeOutgoingGameplayEffectSpec(GE_ZMJZClass, GetAbilityLevel());

			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_Info);
			SpecHandle.Data.Get()->AddDynamicAssetTag(SkillProxyPtr->GetProxyType());
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(UGameplayTagsLibrary::GEData_Duration, DecreamTime);

			const auto GEHandle = ApplyGameplayEffectSpecToOwner(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),
			                                                     GetCurrentActivationInfo(), SpecHandle);

			{
				auto OnActiveGameplayEffectStackChangePtr = GetAbilitySystemComponentFromActorInfo()->
					OnGameplayEffectStackChangeDelegate(GEHandle);
				if (OnActiveGameplayEffectStackChangePtr)
				{
					if (OnActiveGameplayEffectStackChangePtr->IsBound())
					{
					}
					else
					{
						OnActiveGameplayEffectStackChange(GEHandle, 1, 0);
						OnActiveGameplayEffectStackChangePtr->AddUObject(
							this, &ThisClass::OnActiveGameplayEffectStackChange);
					}
				}
			}
			{
				auto OnGameplayEffectRemoved_InfoDelegatePtr = GetAbilitySystemComponentFromActorInfo()->
					OnGameplayEffectRemoved_InfoDelegate(GEHandle);
				if (OnGameplayEffectRemoved_InfoDelegatePtr)
				{
					if (OnGameplayEffectRemoved_InfoDelegatePtr->IsBound())
					{
					}
					else
					{
						OnGameplayEffectRemoved_InfoDelegatePtr->AddUObject(
							this, &ThisClass::OnGameplayEffectRemoved_InfoDelegate);
					}
				}
			}
		}
	}
#endif
}

void USkill_Passive_ZMJZ::OnSendAttack(const FReceivedEventModifyDataCallback& ReceivedEventModifyDataCallback)
{
	if (CharacterPtr)
	{
		PerformAction(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), &CurrentEventData);
	}
}

void USkill_Passive_ZMJZ::DurationDelegate(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Duration)
{
#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
	{
	}
#endif
}

bool USkill_Passive_ZMJZ::OnTimerTaskFinished(UAbilityTask_TimerHelper* TaskPtr)
{
#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
	{
	}
#endif
	return true;
}

void USkill_Passive_ZMJZ::ModifyCharacterData(
	const FGameplayTag& DataSource,
	int32 Value,
	bool bIsClear
)
{
}

void USkill_Passive_ZMJZ::OnActiveGameplayEffectStackChange(
	FActiveGameplayEffectHandle Handle,
	int32 NewStackCount,
	int32 PreviousStackCount)
{
#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
	{
		// 更新每层实际的攻速收益
		auto UpdateGELmbda = [this,NewStackCount]
		{
			FGameplayEffectSpecHandle SpecHandle =
				MakeOutgoingGameplayEffectSpec(GE_ZMJZImpClass, GetAbilityLevel());

			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_Temporary);
			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyItem_PerformSpeed);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(SkillProxyPtr->GetProxyType(), NewStackCount * 10);

			const auto GEHandle = ApplyGameplayEffectSpecToOwner(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),
			                                                     GetCurrentActivationInfo(), SpecHandle);
		};
		if (NewStackCount > PreviousStackCount)
		{
			UpdateGELmbda();
		}
		else if (NewStackCount == PreviousStackCount)
		{
		}
		else
		{
			if (NewStackCount > 0)
			{
				// 当第一层消失时更新后续的消失时间
				FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](auto)
				{
					FGameplayEffectSpecHandle SpecHandle =
						MakeOutgoingGameplayEffectSpec(GE_ZMJZClass, GetAbilityLevel());

					SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_Info);
					SpecHandle.Data.Get()->AddDynamicAssetTag(SkillProxyPtr->GetProxyType());
					SpecHandle.Data.Get()->SetSetByCallerMagnitude(UGameplayTagsLibrary::GEData_Duration,
					                                               SecondaryDecreamTime);
					SpecHandle.Data.Get()->SetStackCount(0);

					const auto GEHandle = ApplyGameplayEffectSpecToOwner(
						GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), SpecHandle);

					return false;
				}), 0);

				// 
				UpdateGELmbda();
			}
			else
			{
			}
		}
	}
#endif
}

void USkill_Passive_ZMJZ::OnGameplayEffectRemoved_InfoDelegate(const FGameplayEffectRemovalInfo&)
{
#if UE_EDITOR || UE_SERVER

	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
	{
		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(GE_ZMJZImpClass, GetAbilityLevel());

		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_RemoveTemporary);
		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyItem_PerformSpeed);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(SkillProxyPtr->GetProxyType(), 0);

		const auto GEHandle = ApplyGameplayEffectSpecToOwner(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),
		                                                     GetCurrentActivationInfo(), SpecHandle);
	}
#endif
}
