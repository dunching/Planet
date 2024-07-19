// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilityTask_FlyAway.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/RootMotionSource.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

#include "Helper_RootMotionSource.h"
#include "CharacterBase.h"

UAbilityTask_FlyAway::UAbilityTask_FlyAway(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	bTickingTask = true;
}

UAbilityTask_FlyAway* UAbilityTask_FlyAway::NewTask(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	float InDuration,
	float InHeight
)
{
	UAbilityTask_FlyAway* MyTask = NewAbilityTask<UAbilityTask_FlyAway>(OwningAbility);

	MyTask->ForceName = TaskInstanceName;
	MyTask->FinishVelocityMode = ERootMotionFinishVelocityMode::SetVelocity;
	MyTask->FinishSetVelocity = FVector::ZeroVector;
	MyTask->FinishClampVelocity = 0.f;

	MyTask->Duration = InDuration;
	MyTask->Height = InHeight;

	return MyTask;
}

void UAbilityTask_FlyAway::Activate()
{
	Super::Activate();

	SharedInitAndApply();
}

void UAbilityTask_FlyAway::SharedInitAndApply()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC && ASC->AbilityActorInfo->MovementComponent.IsValid())
	{
		MovementComponent = Cast<UCharacterMovementComponent>(ASC->AbilityActorInfo->MovementComponent.Get());
		if (MovementComponent)
		{
			ForceName = ForceName.IsNone() ? FName("AbilityTaskApplyRootMotionConstantForce") : ForceName;
			auto RootMotionSourceSPtr = MakeShared<FRootMotionSource_FlyAway>();

			RootMotionSourceSPtr->InstanceName = ForceName;
			RootMotionSourceSPtr->AccumulateMode = ERootMotionAccumulateMode::Override;
			RootMotionSourceSPtr->Priority = ERootMotionSource_Priority::kFlyAway;
			RootMotionSourceSPtr->Duration = Duration;
			RootMotionSourceSPtr->FinishVelocityParams.Mode = FinishVelocityMode;
			RootMotionSourceSPtr->FinishVelocityParams.SetVelocity = FinishSetVelocity;
			RootMotionSourceSPtr->FinishVelocityParams.ClampVelocity = FinishClampVelocity;

			RootMotionSourceSPtr->Height = Height;
			RootMotionSourceSPtr->OriginalPt = ASC->AbilityActorInfo->AvatarActor->GetActorLocation();

			RootMotionSourceID = MovementComponent->ApplyRootMotionSource(RootMotionSourceSPtr);
		}
	}
}

void UAbilityTask_FlyAway::OnDestroy(bool AbilityIsEnding)
{
	if (MovementComponent)
	{
		MovementComponent->RemoveRootMotionSourceByID(RootMotionSourceID);
	}

	Super::OnDestroy(AbilityIsEnding);
}

