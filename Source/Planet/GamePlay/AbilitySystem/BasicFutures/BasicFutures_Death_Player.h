// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"

#include "BasicFuturesBase.h"
#include "GenerateTypes.h"

#include "BasicFutures_Death_Player.generated.h"

class UAnimMontage;

class ACharacterBase;
class UAbilityTask_TimerHelper;

/**
 * 进入濒死状态
 */
UCLASS()
class PLANET_API UBasicFutures_Death_Player : public UBasicFuturesBase
{
	GENERATED_BODY()

public:

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
	)override;

protected:

 	// virtual void InitalDefaultTags()override;

	void PlayMontage(UAnimMontage* CurMontagePtr, float Rate);

	void OnMontageComplete();

	UFUNCTION()
	void OnNotifyBeginReceived(FName NotifyName);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* DeathMontage = nullptr;

	UPROPERTY(Transient)
	UAbilityTask_TimerHelper* TaskHelper = nullptr;
	
	ACharacterBase* CharacterPtr = nullptr;

};