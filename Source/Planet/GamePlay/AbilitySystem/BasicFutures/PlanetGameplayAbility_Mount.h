// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"

#include "BasicFuturesBase.h"
#include "GenerateType.h"

#include "PlanetGameplayAbility_Mount.generated.h"

class AHorseCharacter;

struct FGameplayAbilityTargetData_Mount : public FGameplayAbilityTargetData
{
	AHorseCharacter * HorseCharacterPtr = nullptr;
};

/**
 * Makes the Character try to jump using the standard Character->Jump. This is an example of a non-instanced ability.
 */
UCLASS()
class PLANET_API UPlanetGameplayAbility_Mount : public UBasicFuturesBase
{
	GENERATED_BODY()

public:
	UPlanetGameplayAbility_Mount();

	virtual void PreActivate(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr
	);

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	);

protected:

	void Mount();

	void PlayMontage();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* MontagePtr = nullptr;

	ACharacterBase* CharacterPtr = nullptr;
	
	AHorseCharacter* HorseCharacterPtr = nullptr;

};