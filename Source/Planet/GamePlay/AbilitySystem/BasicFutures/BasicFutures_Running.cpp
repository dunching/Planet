// Copyright 2020 Dan Kestranek.

#include "BasicFutures_Running.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "CharacterBase.h"
#include "ProxyProcessComponent.h"

#include "AssetRefMap.h"
#include "GameplayTagsLibrary.h"
#include "BaseFeatureComponent.h"
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
		AbilityTags.AddTag(UGameplayTagsLibrary::State_Locomotion_Run);
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

			auto ICPtr = CharacterPtr->GetBaseFeatureComponent();
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
			// CharacterPtr->GetBaseFeatureComponent()->SendEvent2Self(
			// 	ModifyPropertyMap,
			// 	UGameplayTagsLibrary::State_Locomotion_Run
			// );

			FGameplayEffectSpecHandle SpecHandle =
				MakeOutgoingGameplayEffectSpec(GE_UGE_RunningClass, GetAbilityLevel());

			SpecHandle.Data.Get()->DynamicGrantedTags.AddTag(UGameplayTagsLibrary::GEData_Info);
			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_Immediate);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(UGameplayTagsLibrary::GEData_MoveSpeed, RunningSpeedOffset.CurrentValue);

			ActiveGameplayEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
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
			// CharacterPtr->GetBaseFeatureComponent()->ClearData2Self(
			// 	ModifyPropertyMap, 
			// 	UGameplayTagsLibrary::State_Locomotion_Run
			// );

			GetAbilitySystemComponentFromActorInfo()->RemoveActiveGameplayEffect(ActiveGameplayEffectHandle);
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
