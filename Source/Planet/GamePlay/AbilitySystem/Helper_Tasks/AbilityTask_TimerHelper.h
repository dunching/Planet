// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "Abilities/Tasks/AbilityTask.h"

#include "AbilityTask_TimerHelper.generated.h"

class UGameplayAbility;

DECLARE_DELEGATE_OneParam(TimerHelper_Finished_Delegate, UAbilityTask_TimerHelper*);

DECLARE_DELEGATE_TwoParams(TimerHelper_Tick_Delegate, UAbilityTask_TimerHelper*, float);

DECLARE_DELEGATE_ThreeParams(TimerHelper_Interval_Tick_Delegate, UAbilityTask_TimerHelper*, float, float);

UCLASS()
class PLANET_API UAbilityTask_TimerHelper : public UAbilityTask
{
	GENERATED_BODY()

public:

	UAbilityTask_TimerHelper(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_TimerHelper* DelayTask(UGameplayAbility* OwningAbility);

	void SetDuration(float InDuration, float InIntervalTime = -1.f);

	// 如果 InIntervalTime > 0；则count会在累计时间大于InIntervalTime才会+1，否则每次tick+1
	void SetCount(int32 InCount, float InIntervalTime = -1.f);
	
	void SetInfinite(float InIntervalTime = -1.f);

	TimerHelper_Finished_Delegate OnFinished;
	
	TimerHelper_Tick_Delegate TickDelegate;

	TimerHelper_Interval_Tick_Delegate DurationIntervalDelegate;

	TimerHelper_Interval_Tick_Delegate InfiniteIntervalDelegate;

	TimerHelper_Interval_Tick_Delegate CountIntervalDelegate;

protected:

	virtual void Activate() override;

	virtual void TickTask(float DeltaTime) override;

	enum class EType
	{
		kDuration,
		kCount,
		kInfinite,
	};

	EType Type = EType::kInfinite;

	float Duration = -1.f;

	float TotalTime = 0.f;
	
	float IntervalTime = -1.f;

	float CurrentIntervalTime = 0.f;

	int32 Count = 1;

	int32 CurrentCount = 0;

};
