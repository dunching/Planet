// Copyright 2020 Dan Kestranek.

#include "PlanetGameplayAbility_Run.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "EquipmentElementComponent.h"
#include "AssetRefMap.h"

UPlanetGameplayAbility_Run::UPlanetGameplayAbility_Run() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPlanetGameplayAbility_Run::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	auto CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
		CharacterPtr->GetCharacterMovement()->MaxWalkSpeed =
			CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().WalkingSpeed.GetCurrentValue() +
			CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().RunningSpeedOffset.GetCurrentValue();
	}
}

void UPlanetGameplayAbility_Run::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData /*= nullptr */)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	auto CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
		CharacterPtr->GetEquipmentItemsComponent()->AddTag(UAssetRefMap::GetInstance()->RunningAbilityTag);
	}
}

void UPlanetGameplayAbility_Run::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	auto CharacterPtr = CastChecked<ACharacterBase>(ActorInfo->AvatarActor.Get());	
	if (CharacterPtr)
	{
		CharacterPtr->GetEquipmentItemsComponent()->RemoveTag(UAssetRefMap::GetInstance()->RunningAbilityTag);
		CharacterPtr->GetCharacterMovement()->MaxWalkSpeed = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().WalkingSpeed.GetCurrentValue();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UPlanetGameplayAbility_Run::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		auto CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
		if (CharacterPtr)
		{
			if (CharacterPtr->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
			{
				auto& PawnDataStructPtr = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
				if (PawnDataStructPtr.PP.GetCurrentValue() >=
					PawnDataStructPtr.RunningConsume.GetCurrentValue())
				{
					return true;
				}
			}
		}
	}

	return false;
}