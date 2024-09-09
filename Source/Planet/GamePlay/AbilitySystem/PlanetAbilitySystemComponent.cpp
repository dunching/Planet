
#include "PlanetAbilitySystemComponent.h"

#include "GameOptions.h"
#include "PlanetGameplayAbility.h"

void UPlanetAbilitySystemComponent::TickComponent(
	float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlanetAbilitySystemComponent::ReplicateContinues_Implementation(
	FGameplayAbilitySpecHandle Handle,
	FGameplayAbilityActivationInfo ActivationInfo, 
	bool bIsContinue
)
{
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(Handle);
	if (AbilitySpec && AbilitySpec->Ability && AbilitySpec->IsActive())
	{
		// Handle non-instanced case, which cannot perform prediction key validation
		if (AbilitySpec->Ability->GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::NonInstanced)
		{
		}
		else
		{
			TArray<UGameplayAbility*> Instances = AbilitySpec->GetAbilityInstances();

			for (auto Instance : Instances)
			{
				if (Instance->GetCurrentActivationInfoRef().bCanBeEndedByOtherInstance)
				{
					Cast<UPlanetGameplayAbility>(Instance)->SetContinuePerformImp(bIsContinue);
				}
			}
		}
	}
}

bool UPlanetAbilitySystemComponent::K2_HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return HasMatchingGameplayTag(TagToCheck);
}

bool UPlanetAbilitySystemComponent::K2_HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return HasAnyMatchingGameplayTags(TagContainer);
}
