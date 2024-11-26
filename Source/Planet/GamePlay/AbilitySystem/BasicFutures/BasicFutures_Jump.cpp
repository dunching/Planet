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

void UBasicFutures_Jump::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	auto CharacterPtrPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtrPtr)
	{
		CharacterPtrPtr->LandedDelegate.AddDynamic(this, &ThisClass::OnLanded);
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

	CharacterPtr->Jump();
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
		return CharacterPtr && CharacterPtr->CanJump();
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
	CharacterPtr->StopJumping();

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UBasicFutures_Jump::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec
)
{
	if (CharacterPtr)
	{
		CharacterPtr->LandedDelegate.RemoveDynamic(this, &ThisClass::OnLanded);
	}
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UBasicFutures_Jump::InitalDefaultTags()
{
	AbilityTags.AddTag(UGameplayTagsSubSystem::Jump);

	// 在运动时不激活
	ActivationBlockedTags.AddTag(UGameplayTagsSubSystem::RootMotion);
}

void UBasicFutures_Jump::OnLanded(const FHitResult& Hit)
{
	K2_CancelAbility();
}
