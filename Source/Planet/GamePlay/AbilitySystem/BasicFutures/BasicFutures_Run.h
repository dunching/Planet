// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"

#include "BasicFuturesBase.h"
#include "GenerateType.h"
#include "BaseData.h"

#include "BasicFutures_Run.generated.h"

class UAbilityTask_TimerHelper;

/**
 *
 */
UCLASS()
class PLANET_API UBasicFutures_Run : public UBasicFuturesBase
{
	GENERATED_BODY()

public:

	UBasicFutures_Run();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void PreActivate(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr
	);

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	);

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

protected:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBaseProperty RunningSpeedOffset;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBaseProperty RunningConsume;

private:

	virtual void InitalDefaultTags() override;

	void IntervalTick(UAbilityTask_TimerHelper*, float Interval, float InDuration);
	
};
