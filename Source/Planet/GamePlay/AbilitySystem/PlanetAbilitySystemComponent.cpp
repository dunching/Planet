
#include "PlanetAbilitySystemComponent.h"

#include "GameOptions.h"

void UPlanetAbilitySystemComponent::TickComponent(
	float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UPlanetAbilitySystemComponent::K2_HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return HasMatchingGameplayTag(TagToCheck);
}

bool UPlanetAbilitySystemComponent::K2_HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return HasAnyMatchingGameplayTags(TagContainer);
}
