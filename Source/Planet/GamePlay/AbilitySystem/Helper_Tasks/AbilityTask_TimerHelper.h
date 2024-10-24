// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "Abilities/Tasks/AbilityTask.h"

#include "AbilityTask_TimerHelper.generated.h"

class UGameplayAbility;

// 是否结束
using TimerHelper_Finished_Delegate = TDelegate<bool(UAbilityTask_TimerHelper*)>;

DECLARE_DELEGATE_TwoParams(TimerHelper_Tick_Delegate, UAbilityTask_TimerHelper*, float);

DECLARE_DELEGATE_ThreeParams(TimerHelper_Interval_Tick_Delegate, UAbilityTask_TimerHelper*, float, float);

UENUM()
enum class ETaskTimerType : uint8
{
	kDuration,
	kCount,
	kInfinite,
};

UCLASS()
class PLANET_API UAbilityTask_TimerHelper : public UAbilityTask
{
	GENERATED_BODY()

public:

	UAbilityTask_TimerHelper(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_TimerHelper* DelayTask(UGameplayAbility* OwningAbility);

	void UpdateDuration();

	void SetDuration(float InDuration, float InIntervalTime = -1.f);

	// 如果 InIntervalTime > 0；则count会在累计时间大于InIntervalTime才会+1，否则每次tick+1
	void SetCount(int32 InCount, float InIntervalTime = -1.f);
	
	// InIntervalTime < 0 
	void SetInfinite(float InIntervalTime = -1.f);

	void SetFinished();
	
	TimerHelper_Finished_Delegate OnFinished;
	
	// 
	TimerHelper_Tick_Delegate TickDelegate;

	// 当前时间 总时间
	TimerHelper_Interval_Tick_Delegate DurationDelegate;

	// 当前时间 间隔时间
	TimerHelper_Interval_Tick_Delegate IntervalDelegate;

protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Activate() override;

	virtual void TickTask(float DeltaTime) override;

	UPROPERTY(Replicated)
	ETaskTimerType Type = ETaskTimerType::kInfinite;
	
	UPROPERTY(Replicated)
	float Duration = -1.f;
	
	UPROPERTY(Replicated)
	float Duration_TotalTime = 0.f;
	
	UPROPERTY(Replicated)
	float IntervalTime = -1.f;
	
	UPROPERTY(Replicated)
	float CurrentIntervalTime = 0.f;
	
	UPROPERTY(Replicated)
	int32 Count = 1;
	
	UPROPERTY(Replicated)
	int32 CurrentCount = 0;

};
