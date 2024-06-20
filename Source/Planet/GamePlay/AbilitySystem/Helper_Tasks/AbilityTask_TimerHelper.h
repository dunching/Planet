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

UCLASS()
class PLANET_API UAbilityTask_TimerHelper : public UAbilityTask
{
	GENERATED_BODY()

public:

	UAbilityTask_TimerHelper(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_TimerHelper* DelayTask(UGameplayAbility* OwningAbility);

	void SetDuration(float InDuration);

	void SetCount(int32 InCount);
	
	void SetInfinite();

	void SetIntervalTime(float InIntervalTime);

	TimerHelper_Finished_Delegate OnFinished;
	
	TimerHelper_Tick_Delegate TickDelegate;

	TimerHelper_Tick_Delegate IntervalDelegate;

protected:

	virtual void Activate() override;

	virtual void TickTask(float DeltaTime) override;

	enum class EType
	{
		kDuration,
		kCount,
		kInfinite_Interval,
	};

	EType Type = EType::kInfinite_Interval;

	float Duration = -1.f;

	float TotalTime = 0.f;
	
	float IntervalTime = -1.f;

	float CurrentIntervalTime = 0.f;

	int32 Count = 1;

	int32 CurrentCount = 0;

};
