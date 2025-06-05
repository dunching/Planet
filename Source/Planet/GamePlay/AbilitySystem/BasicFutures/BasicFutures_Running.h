// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BasicFuturesBase.h"
#include "GenerateTypes.h"
#include "BaseData.h"

#include "BasicFutures_Running.generated.h"

class UAbilityTask_TimerHelper;
class UGE_Running;
class UGE_CancelRunning;

/**
 *
 */
UCLASS()
class PLANET_API UBasicFutures_Running : public UBasicFuturesBase
{
	GENERATED_BODY()

public:

	UBasicFutures_Running();

	virtual void PreActivate(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr
	)override;

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
	) override;

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual void ApplyCost(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo
		) const override;

protected:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBaseProperty RunningSpeedOffset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBaseProperty RunningConsume;

private:

	FActiveGameplayEffectHandle RunningCostGEHandle;
	
	// virtual void InitalDefaultTags() override;

	void IntervalTick(UAbilityTask_TimerHelper*, float Interval, float InDuration);
	
};
