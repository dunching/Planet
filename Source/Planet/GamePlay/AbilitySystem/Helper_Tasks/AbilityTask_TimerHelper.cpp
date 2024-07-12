// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilityTask_TimerHelper.h"

#include <functional>

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

void UAbilityTask_TimerHelper::SetDuration(float InDuration, float InIntervalTime)
{
	Type = EType::kDuration;

	Duration = InDuration;
	IntervalTime = InIntervalTime;
}

void UAbilityTask_TimerHelper::SetCount(int32 InCount, float InIntervalTime)
{
	Type = EType::kCount;

	Count = InCount;
	IntervalTime = InIntervalTime;
}

void UAbilityTask_TimerHelper::SetInfinite(float InIntervalTime)
{
	Type = EType::kInfinite;

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

	auto UpdateIntervalTime = [&] (const std::function<void()>& Lambda = nullptr)
		{
			CurrentIntervalTime += DeltaTime;
			IntervalDelegate.ExecuteIfBound(this, CurrentIntervalTime, IntervalTime);
			if (CurrentIntervalTime >= IntervalTime)
			{
				if (Lambda)
				{
					Lambda();
				}

				CurrentIntervalTime = 0.f;
			}
		};

	switch (Type)
	{
	case EType::kDuration:
	{
		if (IntervalTime > 0.f)
		{
			UpdateIntervalTime();
		}

		if (Duration > 0.f)
		{
			TotalTime += DeltaTime;
			DurationDelegate.ExecuteIfBound(this, TotalTime, Duration);
			if (TotalTime >= Duration)
			{
				OnFinished.ExecuteIfBound(this);
				EndTask();
			}
		}
		else
		{
			EndTask();
		}
	}
	break;
	case EType::kCount:
	{
		if (IntervalTime > 0.f)
		{
			UpdateIntervalTime([&] {
				CurrentCount++;
				});
		}
		else
		{
			CurrentCount++;
		}

		if (Count > 0)
		{
			if (CurrentCount >= Count)
			{
				OnFinished.ExecuteIfBound(this);
				EndTask();
			}
		}
		else
		{
			EndTask();
		}
	}
	break;
	case EType::kInfinite:
	{
		if (IntervalTime > 0.f)
		{
			UpdateIntervalTime();
		}
		else
		{
			EndTask();
		}
	}
	break;
	}
}
