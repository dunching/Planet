// Copyright 2020 Dan Kestranek.

#include "BasicFutures_Run.h"

#include "GameFramework/CharacterMovementComponent.h"
#include <GameFramework/Character.h>

#include "InteractiveSkillComponent.h"
#include "InteractiveToolComponent.h"
#include "InteractiveBaseGAComponent.h"

UBasicFutures_Run::UBasicFutures_Run() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
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

	if (CharacterPtr)
	{
		auto & CharacterAttributesRef = CharacterInterfacePtr->GetCharacterAttributes();
		CharacterPtr->GetCharacterMovement()->MaxWalkSpeed =
			CharacterAttributesRef.MoveSpeed.GetCurrentValue() +
			CharacterAttributesRef.RunningSpeedOffset.GetCurrentValue();
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
	if (CharacterPtr)
	{
		CharacterPtr->GetCharacterMovement()->MaxWalkSpeed = 
			CharacterInterfacePtr->GetCharacterAttributes().MoveSpeed.GetCurrentValue();
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
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		if (CharacterPtr)
		{
			if (CharacterPtr->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
			{
				auto& PawnDataStructPtr = CharacterInterfacePtr->GetCharacterAttributes();
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
