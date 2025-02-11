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

void UBasicFutures_Running::InitalDefaultTags()
{
	Super::InitalDefaultTags();

	if (GetWorldImp())
	{
		// AbilityTags.AddTag(UGameplayTagsLibrary::State_Locomotion_Run);
		ActivationOwnedTags.AddTag(UGameplayTagsLibrary::State_Locomotion_Run);
	}
}

void UBasicFutures_Running::IntervalTick(UAbilityTask_TimerHelper*, float Interval, float InDuration)
{
	if (Interval > InDuration)
	{
		auto CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		if (
			CharacterAttributes->GetPP() >=
			RunningConsume.GetCurrentValue()
		)
		{
			TMap<ECharacterPropertyType, FBaseProperty> ModifyPropertyMap;

			FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(
				CharacterPtr);

			GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

			FGAEventData GAEventData(CharacterPtr, CharacterPtr);

			GAEventData.DataSource = UGameplayTagsLibrary::DataSource_Character;

			GAEventData.DataModify.Add(ECharacterPropertyType::PP, -RunningConsume.GetCurrentValue());

			GAEventDataPtr->DataAry.Add(GAEventData);

			auto ICPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
			ICPtr->SendEventImp(GAEventDataPtr);
		}
		else
		{
			K2_CancelAbility();
		}
	}
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
			// //
			// {
			// 	auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
			// 	TaskPtr->SetInfinite(1.f);
			// 	TaskPtr->IntervalDelegate.BindUObject(this, &ThisClass::IntervalTick);
			// 	TaskPtr->ReadyForActivation();
			// }
			//
			// TMap<ECharacterPropertyType, FBaseProperty> ModifyPropertyMap;
			//
			// ModifyPropertyMap.Add(
			// 	ECharacterPropertyType::MoveSpeed,
			// 	RunningSpeedOffset.GetCurrentValue()
			// );
			//
			// CharacterPtr->GetCharacterAbilitySystemComponent()->SendEvent2Self(
			// 	ModifyPropertyMap,
			// 	UGameplayTagsLibrary::State_Locomotion_Run
			// );

			FGameplayEffectSpecHandle SpecHandle =
				MakeOutgoingGameplayEffectSpec(GE_RunningClass, GetAbilityLevel());

			// SpecHandle.Data.Get()->DynamicGrantedTags.AddTag(UGameplayTagsLibrary::GEData_Info);
			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_Temporary);
			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyItem_MoveSpeed);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(UGameplayTagsLibrary::DataSource_Character, RunningSpeedOffset.CurrentValue);

			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
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
			// TMap<ECharacterPropertyType, FBaseProperty> ModifyPropertyMap;
			//
			// ModifyPropertyMap.Add(ECharacterPropertyType::MoveSpeed, 0);
			//
			// CharacterPtr->GetCharacterAbilitySystemComponent()->ClearData2Self(
			// 	ModifyPropertyMap, 
			// 	UGameplayTagsLibrary::State_Locomotion_Run
			// );

			FGameplayEffectSpecHandle SpecHandle =
				MakeOutgoingGameplayEffectSpec(GE_CancelRunningClass, GetAbilityLevel());

			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_RemoveTemporary);
			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyItem_MoveSpeed);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(UGameplayTagsLibrary::DataSource_Character, RunningSpeedOffset.CurrentValue);

			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
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
				CharacterAttributes->GetPP() >=
				RunningConsume.GetCurrentValue()
			)
			{
				return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
			}
		}
	}

	return false;
}
