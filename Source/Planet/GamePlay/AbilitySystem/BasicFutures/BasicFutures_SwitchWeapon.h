
#pragma once

#include "CoreMinimal.h"

#include "PlanetGameplayAbility.h"
#include "BasicFuturesBase.h"

#include "BasicFutures_SwitchWeapon.generated.h"

class UAnimMontage;
class ACharacterBase;

/**
 * 受击时的“僵直效果”
 */

UCLASS()
class PLANET_API UBasicFutures_SwitchWeapon : public UBasicFuturesBase
{
	GENERATED_BODY()

public:

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

private:
	
	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual void ApplyCooldown(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo
	) const override;

	virtual void ApplyCost(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo
	) const override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	int32 CD = 2;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	int32 Cost = 10;
};
