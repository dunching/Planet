// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"

#include "BasicFuturesBase.h"
#include "GenerateType.h"

#include "BasicFutures_Death.generated.h"

class UAnimMontage;

class ACharacterBase;

UCLASS()
class PLANET_API UBasicFutures_Death : public UBasicFuturesBase
{
	GENERATED_BODY()

public:

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

protected:

	virtual void InitalDefaultTags()override;

	void PlayMontage(UAnimMontage* CurMontagePtr, float Rate);

	void OnMontageComplete();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* DeathMontage = nullptr;

	ACharacterBase* CharacterPtr = nullptr;

};