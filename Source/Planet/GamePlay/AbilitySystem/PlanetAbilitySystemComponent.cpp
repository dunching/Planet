
#include "PlanetAbilitySystemComponent.h"

#include "GameOptions.h"
#include "PlanetGameplayAbility.h"

void UPlanetAbilitySystemComponent::TickComponent(
	float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlanetAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	if (GameplayEventDataMap.Find(AbilitySpec.InputID))
	{
		if (AbilitySpec.Ability)
		{
			AbilitySpec.GameplayEventData = MakeShared<FGameplayEventData>();

			*AbilitySpec.GameplayEventData = GameplayEventDataMap[AbilitySpec.InputID];
			GameplayEventDataMap.Remove(AbilitySpec.InputID);
		}
	}

	Super::OnGiveAbility(AbilitySpec);
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
				Cast<UPlanetGameplayAbility>(Instance)->SetContinuePerformImp(bIsContinue);
			}
		}
	}
}

void UPlanetAbilitySystemComponent::ReplicateEventData_Implementation(
	int32 InputID,
	const FGameplayEventData& TriggerEventData
)
{
	GameplayEventDataMap.Add(InputID, TriggerEventData);
}

bool UPlanetAbilitySystemComponent::K2_HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return HasMatchingGameplayTag(TagToCheck);
}

bool UPlanetAbilitySystemComponent::K2_HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return HasAnyMatchingGameplayTags(TagContainer);
}
