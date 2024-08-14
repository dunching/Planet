// Copyright 2020 Dan Kestranek.

#include "BasicFutures_Jump.h"

#include "GenerateType.h"
#include "CharacterBase.h"
#include "Planet_Tools.h"
#include "GameplayTagsSubSystem.h"

UBasicFutures_Jump::UBasicFutures_Jump() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBasicFutures_Jump::PostCDOContruct()
{
	Super::PostCDOContruct();

	if (GetWorldImp())
	{
		AbilityTags.AddTag(UGameplayTagsSubSystem::GetInstance()->Jump);

		FAbilityTriggerData AbilityTriggerData;

		AbilityTriggerData.TriggerTag = UGameplayTagsSubSystem::GetInstance()->Jump;
		AbilityTriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;

		AbilityTriggers.Add(AbilityTriggerData);
	}
}

void UBasicFutures_Jump::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	Character->Jump();
}

bool UBasicFutures_Jump::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		const ACharacterBase* Character = CastChecked<ACharacterBase>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
		return Character && Character->CanJump();
	}
	return false;
}

void UBasicFutures_Jump::CancelAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility
)
{
	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	Character->StopJumping();

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}