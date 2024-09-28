// Copyright 2020 Dan Kestranek.

#include "BasicFutures_Run.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "CharacterBase.h"
#include "UnitProxyProcessComponent.h"

#include "AssetRefMap.h"
#include "GameplayTagsSubSystem.h"
#include "BaseFeatureComponent.h"
#include "CharacterAttributesComponent.h"
#include "CharacterAttibutes.h"
#include "Planet_Tools.h"

UBasicFutures_Run::UBasicFutures_Run() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBasicFutures_Run::InitialTags()
{
	Super::InitialTags();

	if (GetWorldImp())
	{
		AbilityTags.AddTag(UGameplayTagsSubSystem::GetInstance()->Running);
		ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->Running);
	}
}

void UBasicFutures_Run::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	auto CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
	}
}

void UBasicFutures_Run::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	auto CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
#if UE_EDITOR || UE_SERVER
		if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
		{
			TMap<ECharacterPropertyType, FBaseProperty> ModifyPropertyMap;

			ModifyPropertyMap.Add(
				ECharacterPropertyType::MoveSpeed,
				CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().RunningSpeedOffset.GetCurrentValue()
			);

			CharacterPtr->GetBaseFeatureComponent()->SendEvent2Self(ModifyPropertyMap, UGameplayTagsSubSystem::GetInstance()->Running);
		}
#endif
	}
}

void UBasicFutures_Run::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	auto CharacterPtr = CastChecked<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
#if UE_EDITOR || UE_SERVER
		if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
		{
		TMap<ECharacterPropertyType, FBaseProperty> ModifyPropertyMap;

		ModifyPropertyMap.Add(ECharacterPropertyType::MoveSpeed, 0);

		CharacterPtr->GetBaseFeatureComponent()->ClearData2Self(ModifyPropertyMap, UGameplayTagsSubSystem::GetInstance()->Running);
		}
#endif
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UBasicFutures_Run::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	auto CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
		if (CharacterPtr->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
		{
			auto CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
			if (CharacterAttributes.PP.GetCurrentValue() >=
				CharacterAttributes.RunningConsume.GetCurrentValue())
			{
				return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
			}
		}
	}

	return false;
}
