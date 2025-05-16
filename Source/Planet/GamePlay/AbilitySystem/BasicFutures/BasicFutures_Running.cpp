// Copyright 2020 Dan Kestranek.

#include "BasicFutures_Running.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "CharacterBase.h"
#include "ProxyProcessComponent.h"

#include "AssetRefMap.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAbilitySystemComponent.h"
#include "CharacterAttributesComponent.h"
#include "GE_Common.h"
#include "Planet_Tools.h"
#include "AbilityTask_TimerHelper.h"
#include "AS_Character.h"

UBasicFutures_Running::UBasicFutures_Running() :
                                               Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBasicFutures_Running::IntervalTick(
	UAbilityTask_TimerHelper*,
	float Interval,
	float InDuration
	)
{
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (Interval > InDuration)
		{
			// 停止输入时
			if (CharacterPtr->GetCharacterMovement()->GetCurrentAcceleration().Length() < 10.f)
			{
				K2_CancelAbility();
				return;
			}

			auto CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
			if (
				CharacterAttributes->GetStamina() >=
				RunningConsume.GetCurrentValue()
			)
			{
			}
			else
			{
				K2_CancelAbility();
			}
		}
	}
#endif
}

void UBasicFutures_Running::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
	)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UBasicFutures_Running::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (CharacterPtr)
	{
#if UE_EDITOR || UE_SERVER
		if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
		{
			CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());
			// 加速的效果
			{
				FGameplayEffectSpecHandle SpecHandle =
					MakeOutgoingGameplayEffectSpec(UAssetRefMap::GetInstance()->OnceGEClass, GetAbilityLevel());

				// SpecHandle.Data.Get()->DynamicGrantedTags.AddTag(UGameplayTagsLibrary::GEData_Info);
				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_Temporary);
				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyItem_MoveSpeed);
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
				                                               UGameplayTagsLibrary::BaseFeature_Run,
				                                               RunningSpeedOffset.CurrentValue
				                                              );

				ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
			}
			// 消耗的效果
			{
				FGameplayEffectSpecHandle SpecHandle =
					MakeOutgoingGameplayEffectSpec(UAssetRefMap::GetInstance()->ForeverGEClass, GetAbilityLevel());

				// SpecHandle.Data.Get()->DynamicGrantedTags.AddTag(UGameplayTagsLibrary::GEData_Info);
				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);
				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyItem_PP);
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
				                                               UGameplayTagsLibrary::GEData_ModifyItem_PP,
				                                               -RunningConsume.CurrentValue
				                                              );

				RunningCostGEHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
			}
			{
				auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
				TaskPtr->SetInfinite(1.f);
				TaskPtr->IntervalDelegate.BindUObject(this, &ThisClass::IntervalTick);
				TaskPtr->ReadyForActivation();
			}
		}
#endif
	}
}

void UBasicFutures_Running::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	if (CharacterPtr)
	{
#if UE_EDITOR || UE_SERVER
		if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
		{
			FGameplayEffectSpecHandle SpecHandle =
				MakeOutgoingGameplayEffectSpec(UAssetRefMap::GetInstance()->OnceGEClass, GetAbilityLevel());

			// SpecHandle.Data.Get()->DynamicGrantedTags.AddTag(UGameplayTagsLibrary::GEData_Info);
			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_RemoveTemporary);
			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyItem_MoveSpeed);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::BaseFeature_Run,
			                                               RunningSpeedOffset.CurrentValue
			                                              );

			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);

			BP_RemoveGameplayEffectFromOwnerWithHandle(RunningCostGEHandle);
		}
#endif
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UBasicFutures_Running::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
	) const
{
	if (CharacterPtr)
	{
		if (CharacterPtr->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
		{
			auto CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
			if (
				CharacterAttributes->GetStamina() >=
				RunningConsume.GetCurrentValue()
			)
			{
				return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
			}
		}
	}

	return false;
}

void UBasicFutures_Running::ApplyCost(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo
	) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
}
