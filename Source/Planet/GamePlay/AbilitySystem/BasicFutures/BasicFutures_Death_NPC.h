// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"

#include "BasicFuturesBase.h"
#include "GenerateTypes.h"

#include "BasicFutures_Death_NPC.generated.h"

class UAnimMontage;

class ACharacterBase;
class UAbilityTask_TimerHelper;

/**
 * 进入濒死状态
 */
UCLASS()
class PLANET_API UBasicFutures_Death_NPC : public UBasicFuturesBase
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

	UFUNCTION()
	bool DestroyAvatar(UAbilityTask_TimerHelper*TaskPtr) ;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* DeathMontage = nullptr;

	UPROPERTY(Transient)
	UAbilityTask_TimerHelper* TaskHelper = nullptr;
	
	/**
	 * 在蒙太奇播放完成之后第几秒后销魂Actor -1不销毁
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	int32 DestroyInSecond = -1;

	ACharacterBase* CharacterPtr = nullptr;

};