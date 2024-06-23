// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilityTask_TimerHelper.h"
#include "TimerManager.h"
#include "Engine/World.h"

UAbilityTask_TimerHelper::UAbilityTask_TimerHelper(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	bTickingTask = true;
}

UAbilityTask_TimerHelper* UAbilityTask_TimerHelper::DelayTask(UGameplayAbility* OwningAbility)
{
	UAbilityTask_TimerHelper* MyObj = NewAbilityTask<UAbilityTask_TimerHelper>(OwningAbility);

	return MyObj;
}

void UAbilityTask_TimerHelper::SetDuration(float InDuration)
{
	Type = EType::kDuration;

	Duration = InDuration;
}

void UAbilityTask_TimerHelper::SetCount(int32 InCount)
{
	Type = EType::kCount;

	Count = InCount;
}

void UAbilityTask_TimerHelper::SetInfinite()
{
	Type = EType::kInfinite_Interval;
}

void UAbilityTask_TimerHelper::SetIntervalTime(float InIntervalTime)
{
	IntervalTime = InIntervalTime;
}

void UAbilityTask_TimerHelper::Activate()
{
	Super::Activate();
}

void UAbilityTask_TimerHelper::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	TickDelegate.ExecuteIfBound(this, DeltaTime);

	switch (Type)
	{
	case EType::kDuration:
	{
		if (Duration > 0.f)
		{
			TotalTime += DeltaTime;
			if (TotalTime >= Duration)
			{
				OnFinished.ExecuteIfBound(this);
				EndTask();
			}
		}
	}
	break;
	case EType::kCount:
	{
		CurrentCount++;
		if (Count > 0)
		{
			if (CurrentCount >= Count)
			{
				OnFinished.ExecuteIfBound(this);
				EndTask();
			}
		}
	}
	break;
	case EType::kInfinite_Interval:
	{
		CurrentIntervalTime += DeltaTime;
		IntervalDelegate.ExecuteIfBound(this, CurrentIntervalTime, IntervalTime);
		if (CurrentIntervalTime >= IntervalTime)
		{
			CurrentIntervalTime = 0.f;
		}
	}
	break;
	}
}
