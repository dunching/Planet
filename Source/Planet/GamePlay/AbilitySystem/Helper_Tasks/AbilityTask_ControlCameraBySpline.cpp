// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilityTask_ControlCameraBySpline.h"

#include "GameFramework/RootMotionSource.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include <Components/SplineComponent.h>
#include <GameFramework/SpringArmComponent.h>

#include "Helper_RootMotionSource.h"
#include "CharacterBase.h"
#include "SPlineActor.h"
#include "CameraTrailHelper.h"

UAbilityTask_ControlCameraBySpline::UAbilityTask_ControlCameraBySpline(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	bTickingTask = true;
}

UAbilityTask_ControlCameraBySpline* UAbilityTask_ControlCameraBySpline::NewTask
(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	float Duration,
	ACameraTrailHelper* InCameraTrailHelperPtr,
	ACharacterBase* InTargetCharacterPtr,
	int32 StartPtIndex,
	int32 EndPtIndex
)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Duration(Duration);

	const auto StartDistance =
		InCameraTrailHelperPtr->SplineComponentPtr->GetDistanceAlongSplineAtSplinePoint(StartPtIndex);

	if (EndPtIndex < 0)
	{
		EndPtIndex = InCameraTrailHelperPtr->SplineComponentPtr->GetNumberOfSplinePoints() - 1;
	}

	const auto EndDistance =
		InCameraTrailHelperPtr->SplineComponentPtr->GetDistanceAlongSplineAtSplinePoint(EndPtIndex);

	return ThisClass::NewTask(
		OwningAbility,
		TaskInstanceName,
		Duration,
		InCameraTrailHelperPtr,
		InTargetCharacterPtr,
		StartDistance,
		EndDistance
	);
}

UAbilityTask_ControlCameraBySpline* UAbilityTask_ControlCameraBySpline::NewTask(
	UGameplayAbility* OwningAbility, 
	FName TaskInstanceName, 
	float Duration, 
	ACameraTrailHelper* InCameraTrailHelperPtr, 
	ACharacterBase* InTargetCharacterPtr, 
	float StartDistance,
	float EndDistance
)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Duration(Duration);

	auto MyTask = NewAbilityTask<UAbilityTask_ControlCameraBySpline>(OwningAbility, TaskInstanceName);

	MyTask->Duration = Duration;
	MyTask->CameraTrailHelperPtr = InCameraTrailHelperPtr;
	MyTask->TargetCharacterPtr = InTargetCharacterPtr;

	MyTask->StartDistance = StartDistance;
	MyTask->EndDistance = EndDistance;

	return MyTask;
}

void UAbilityTask_ControlCameraBySpline::Activate()
{
	Super::Activate();

	StartTime = GetWorld()->GetTimeSeconds();
	EndTime = StartTime + Duration;

	CurrentTime = 0;
}

void UAbilityTask_ControlCameraBySpline::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	auto AdjustCameraOffset = [this] {
		const float MoveFraction = CurrentTime / Duration;

		const auto LerpValue = FMath::Lerp(StartDistance, EndDistance, MoveFraction);

		const auto Pt = CameraTrailHelperPtr->SplineComponentPtr->GetWorldLocationAtDistanceAlongSpline(LerpValue);

		TargetCharacterPtr->GetCameraBoom()->TargetOffset = Pt - TargetCharacterPtr->GetActorLocation();
		};

	CurrentTime += DeltaTime;
	if (CurrentTime > Duration)
	{
		CurrentTime = Duration;
		AdjustCameraOffset();

		OnFinish.ExecuteIfBound();

		EndTask();
	}
	else
	{
		AdjustCameraOffset();
	}
}

void UAbilityTask_ControlCameraBySpline::OnDestroy(bool AbilityIsEnding)
{
	Super::OnDestroy(AbilityIsEnding);
}

