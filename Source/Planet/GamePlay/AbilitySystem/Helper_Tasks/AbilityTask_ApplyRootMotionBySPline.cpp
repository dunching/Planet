// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilityTask_ApplyRootMotionBySPline.h"

#include "GameFramework/RootMotionSource.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include <Components/SplineComponent.h>

#include "Helper_RootMotionSource.h"
#include "CharacterBase.h"
#include "SPlineActor.h"

UAbilityTask_ApplyRootMotionBySPline* UAbilityTask_ApplyRootMotionBySPline::NewTask
(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	float Duration,
	ASPlineActor* InSPlineActorPtr,
	ACharacterBase* InTargetCharacterPtr,
	int32 StartPtIndex,
	int32 EndPtIndex
)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Duration(Duration);

	UAbilityTask_ApplyRootMotionBySPline* MyTask = NewAbilityTask<UAbilityTask_ApplyRootMotionBySPline>(OwningAbility, TaskInstanceName);

	MyTask->ForceName = TaskInstanceName;
	MyTask->FinishVelocityMode = ERootMotionFinishVelocityMode::SetVelocity;
	MyTask->FinishSetVelocity = FVector::ZeroVector;
	MyTask->FinishClampVelocity = 0.f;

	MyTask->Duration = Duration;
	MyTask->SPlineActorPtr = InSPlineActorPtr;
	MyTask->TargetCharacterPtr = InTargetCharacterPtr;
	MyTask->StartPtIndex = StartPtIndex;
	MyTask->EndPtIndex = EndPtIndex;

	return MyTask;
}

void UAbilityTask_ApplyRootMotionBySPline::Activate()
{
	Super::Activate();

	SharedInitAndApply();
}

void UAbilityTask_ApplyRootMotionBySPline::SharedInitAndApply()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC && ASC->AbilityActorInfo->MovementComponent.IsValid())
	{
		MovementComponent = Cast<UCharacterMovementComponent>(ASC->AbilityActorInfo->MovementComponent.Get());
		StartTime = GetWorld()->GetTimeSeconds(); 
		EndTime = StartTime + Duration;

		if (MovementComponent)
		{
			ForceName = ForceName.IsNone() ? FName("AbilityTaskApplyRootMotionConstantForce") : ForceName;
			TSharedPtr<FRootMotionSource_BySpline> RootMotionSource = MakeShared<FRootMotionSource_BySpline>();

			RootMotionSource->InstanceName = ForceName;
			RootMotionSource->AccumulateMode = ERootMotionAccumulateMode::Override;
			RootMotionSource->Priority = ERootMotionSource_Priority::kMove;
			RootMotionSource->Duration = Duration;
			RootMotionSource->FinishVelocityParams.Mode = FinishVelocityMode;
			RootMotionSource->FinishVelocityParams.SetVelocity = FinishSetVelocity;
			RootMotionSource->FinishVelocityParams.ClampVelocity = FinishClampVelocity;

			RootMotionSource->SPlineActorPtr = SPlineActorPtr;
			RootMotionSource->TargetCharacterPtr = TargetCharacterPtr;

			RootMotionSource->StartPtIndex = StartPtIndex;
			RootMotionSource->StartDistance = 
				SPlineActorPtr->SplineComponentPtr->GetDistanceAlongSplineAtSplinePoint(RootMotionSource->StartPtIndex);
			if (EndPtIndex < 0)
			{
				RootMotionSource->EndPtIndex = SPlineActorPtr->SplineComponentPtr->GetNumberOfSplinePoints() - 1;
			}
			else
			{
				RootMotionSource->EndPtIndex = EndPtIndex;
			}
			RootMotionSource->EndDistance =
				SPlineActorPtr->SplineComponentPtr->GetDistanceAlongSplineAtSplinePoint(RootMotionSource->EndPtIndex);

			RootMotionSourceID = MovementComponent->ApplyRootMotionSource(RootMotionSource);
		}
	}
}

void UAbilityTask_ApplyRootMotionBySPline::TickTask(float DeltaTime)
{
	if (bIsFinished)
	{
		return;
	}

	Super::TickTask(DeltaTime);

	AActor* MyActor = GetAvatarActor();
	if (MyActor)
	{
		const bool bTimedOut = HasTimedOut();
		const bool bIsInfiniteDuration = Duration < 0.f;

		if (!bIsInfiniteDuration && bTimedOut)
		{
			// Task has finished
			bIsFinished = true;
			if (!bIsSimulating)
			{
				MyActor->ForceNetUpdate();
				if (ShouldBroadcastAbilityTaskDelegates())
				{
					OnFinish.ExecuteIfBound();
				}
				EndTask();
			}
		}
	}
	else
	{
		bIsFinished = true;
		EndTask();
	}
}

void UAbilityTask_ApplyRootMotionBySPline::OnDestroy(bool AbilityIsEnding)
{
	if (MovementComponent)
	{
		MovementComponent->RemoveRootMotionSourceByID(RootMotionSourceID);
	}

	Super::OnDestroy(AbilityIsEnding);
}

