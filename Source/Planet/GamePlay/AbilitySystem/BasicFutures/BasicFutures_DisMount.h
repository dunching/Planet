// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"

#include "BasicFuturesBase.h"
#include "GenerateType.h"

#include "BasicFutures_DisMount.generated.h"

class AHorseCharacter;

struct FGameplayAbilityTargetData_DisMount : public FGameplayAbilityTargetData
{
	AHorseCharacter* HorseCharacterPtr = nullptr;
};

/**
 * Makes the Character try to jump using the standard Character->Jump. This is an example of a non-instanced ability.
 */
UCLASS()
class PLANET_API UBasicFutures_DisMount : public UBasicFuturesBase
{
	GENERATED_BODY()

public:

	UBasicFutures_DisMount();

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

	void Perform();

	void PlayMontage();

	void OnMontageComplete();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* MontagePtr = nullptr;

	ACharacterBase* CharacterPtr = nullptr;
	
	AHorseCharacter* HorseCharacterPtr = nullptr;

};
