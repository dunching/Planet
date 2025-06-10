#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbilityTypes.h"

#include "PlanetGameplayAbility.h"

#include "GenerateTypes.h"

#include "PlanetGameplayAbilityBase.generated.h"

UCLASS()
class PLANET_API UPlanetGameplayAbilityBase : public UPlanetGameplayAbility
{
	GENERATED_BODY()

public:
	
	virtual UGameplayEffect* GetCooldownGameplayEffect() const override;

	virtual UGameplayEffect* GetCostGameplayEffect() const override;

	virtual void ApplyCost(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo
		) const override;

	virtual bool CheckCooldown(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
		) const override;

	virtual const FGameplayTagContainer* GetCooldownTags() const override;

	virtual bool CheckCost(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
		) const override;

	/**
	 * 
	 * @param AbilityHandle 
	 * @param ActorInfo 
	 * @param ActivationInfo 
	 * @param SpecHandle 
	 * @param TargetData 
	 * @return 
	 */
	TArray<FActiveGameplayEffectHandle> MyApplyGameplayEffectSpecToTarget(
		const FGameplayAbilitySpecHandle AbilityHandle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FGameplayEffectSpecHandle SpecHandle,
		const FGameplayAbilityTargetDataHandle& TargetData
		) const;

	virtual void ApplyCostImp(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const TMap<FGameplayTag, int32>& CostMap
		) const;

	virtual TMap<FGameplayTag, int32>GetCostMap()const ;
	
};
