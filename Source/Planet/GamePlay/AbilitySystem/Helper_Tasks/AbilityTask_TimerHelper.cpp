// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilityTask_TimerHelper.h"

#include <functional>

#include "Net/UnrealNetwork.h"
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

void UAbilityTask_TimerHelper::UpdateDuration()
{
	Duration_TotalTime = 0.f;
}

void UAbilityTask_TimerHelper::SetDuration(float InDuration, float InIntervalTime)
{
	Type = ETaskTimerType::kDuration;

	Duration = InDuration;
	IntervalTime = InIntervalTime;
}

void UAbilityTask_TimerHelper::SetCount(int32 InCount, float InIntervalTime)
{
	Type = ETaskTimerType::kCount;

	Count = InCount;
	IntervalTime = InIntervalTime;
}

void UAbilityTask_TimerHelper::SetInfinite(float InIntervalTime)
{
	Type = ETaskTimerType::kInfinite;

	IntervalTime = InIntervalTime;
}

void UAbilityTask_TimerHelper::SetFinished()
{
	Duration_TotalTime = Duration;
}

void UAbilityTask_TimerHelper::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Type);
	DOREPLIFETIME(ThisClass, Duration);
	DOREPLIFETIME(ThisClass, Duration_TotalTime);
	DOREPLIFETIME(ThisClass, IntervalTime);
	DOREPLIFETIME(ThisClass, CurrentIntervalTime);
	DOREPLIFETIME(ThisClass, Count);
	DOREPLIFETIME(ThisClass, CurrentCount);
}

void UAbilityTask_TimerHelper::Activate()
{
	Super::Activate();
}

void UAbilityTask_TimerHelper::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	TickDelegate.ExecuteIfBound(this, DeltaTime);

	auto UpdateIntervalTime = [&](const std::function<void()>& Lambda = nullptr)
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
	case ETaskTimerType::kDuration:
	{
		if (IntervalTime > 0.f)
		{
			UpdateIntervalTime();
		}

		if (Duration > 0.f)
		{
			Duration_TotalTime += DeltaTime;
			DurationDelegate.ExecuteIfBound(this, Duration_TotalTime, Duration);
			if (Duration_TotalTime >= Duration)
			{
				if (OnFinished.IsBound())
				{
					if (OnFinished.Execute(this))
					{
						EndTask();
					}
				}
				else
				{
					EndTask();
				}
			}
		}
		else
		{
			EndTask();
		}
	}
	break;
	case ETaskTimerType::kCount:
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
				if (OnFinished.IsBound())
				{
					if (OnFinished.Execute(this))
					{
						EndTask();
					}
				}
				else
				{
					EndTask();
				}
			}
		}
		else
		{
			EndTask();
		}
	}
	break;
	case ETaskTimerType::kInfinite:
	{
		UpdateIntervalTime();
	}
	break;
	}
}
