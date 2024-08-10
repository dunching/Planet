// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilityTask_Tornado.h"

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

UAbilityTask_Tornado::UAbilityTask_Tornado(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	bTickingTask = true;
}

UAbilityTask_Tornado* UAbilityTask_Tornado::NewTask(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	TWeakObjectPtr<ATornado> InTornadoPtr,
	ACharacterBase* InTargetCharacterPtr
)
{
	UAbilityTask_Tornado* MyTask = NewAbilityTask<UAbilityTask_Tornado>(OwningAbility);

	MyTask->ForceName = TaskInstanceName;
	MyTask->FinishVelocityMode = ERootMotionFinishVelocityMode::SetVelocity;
	MyTask->FinishSetVelocity = FVector::ZeroVector;
	MyTask->FinishClampVelocity = 0.f;

	MyTask->TornadoPtr = InTornadoPtr;
	MyTask->TargetCharacterPtr = InTargetCharacterPtr;

	return MyTask;
}

void UAbilityTask_Tornado::Activate()
{
	Super::Activate();

	SharedInitAndApply();
}

void UAbilityTask_Tornado::SharedInitAndApply()
{
	if (TargetCharacterPtr && TargetCharacterPtr->GetCharacterMovement())
	{
		MovementComponent = TargetCharacterPtr->GetCharacterMovement();

		if (MovementComponent)
		{
			ForceName = ForceName.IsNone() ? FName("AbilityTaskApplyRootMotionConstantForce") : ForceName;
			TSharedPtr<FRootMotionSource_ByTornado> RootMotionSource = MakeShared<FRootMotionSource_ByTornado>();

			RootMotionSource->InstanceName = ForceName;
			RootMotionSource->AccumulateMode = ERootMotionAccumulateMode::Override;
			RootMotionSource->Priority = ERootMotionSource_Priority::kFlyAway;
			RootMotionSource->Duration = -1.f;
			RootMotionSource->FinishVelocityParams.Mode = FinishVelocityMode;
			RootMotionSource->FinishVelocityParams.SetVelocity = FinishSetVelocity;
			RootMotionSource->FinishVelocityParams.ClampVelocity = FinishClampVelocity;

			RootMotionSource->TornadoPtr = TornadoPtr;
			RootMotionSource->TargetCharacterPtr = TargetCharacterPtr;

			RootMotionSourceID = MovementComponent->ApplyRootMotionSource(RootMotionSource);
		}
	}
}

void UAbilityTask_Tornado::OnDestroy(bool AbilityIsEnding)
{
	if (MovementComponent)
	{
		MovementComponent->RemoveRootMotionSourceByID(RootMotionSourceID);
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UAbilityTask_Tornado::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	const bool bTimedOut = HasTimedOut();

	if (bTimedOut)
	{
		OnFinish.ExecuteIfBound();

		EndTask();
	}
}

