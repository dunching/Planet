
#include "PlanetAbilitySystemComponent.h"

bool UPlanetAbilitySystemComponent::K2_HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return HasMatchingGameplayTag(TagToCheck);
}

bool UPlanetAbilitySystemComponent::K2_HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return HasAnyMatchingGameplayTags(TagContainer);
}

