// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilityTask_ARM_MoveToForce.h"

#include "GameFramework/RootMotionSource.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"


#include "Helper_RootMotionSource.h"

UAbilityTask_ARM_MoveToForce::UAbilityTask_ARM_MoveToForce(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAbilityTask_ARM_MoveToForce* UAbilityTask_ARM_MoveToForce::ApplyRootMotionMoveToForce
(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	FVector StartLocation,
	FVector TargetLocation,
	float InDuration
)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Duration(InDuration);

	UAbilityTask_ARM_MoveToForce* MyTask = NewAbilityTask<UAbilityTask_ARM_MoveToForce>(OwningAbility, TaskInstanceName);

	MyTask->Priority = ERootMotionSource_Priority::kMove;
	MyTask->ForceName = TaskInstanceName;
	MyTask->StartLocation = StartLocation;
	MyTask->TargetLocation = TargetLocation;
	MyTask->Duration = InDuration;
	MyTask->FinishVelocityMode = ERootMotionFinishVelocityMode::ClampVelocity;
	MyTask->FinishClampVelocity = 200.f;

	return MyTask;
}

void UAbilityTask_ARM_MoveToForce::Activate()
{
	Super::Activate();
}

void UAbilityTask_ARM_MoveToForce::SharedInitAndApply()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC && ASC->AbilityActorInfo->MovementComponent.IsValid())
	{
		MovementComponent = Cast<UCharacterMovementComponent>(ASC->AbilityActorInfo->MovementComponent.Get());
		StartTime = GetWorld()->GetTimeSeconds();
		EndTime = StartTime + Duration;

		if (MovementComponent)
		{
			ForceName = ForceName.IsNone() ? FName("AbilityTask_MoveToForce") : ForceName;
			auto ConstantForce = MakeShared<FRootMotionSource_MoveToForce>();
			ConstantForce->InstanceName = ForceName;
			ConstantForce->AccumulateMode = ERootMotionAccumulateMode::Override;
			ConstantForce->Priority = ERootMotionSource_Priority::kMove;
			ConstantForce->StartLocation = StartLocation;
			ConstantForce->TargetLocation = TargetLocation;
			ConstantForce->Duration = Duration;
			ConstantForce->FinishVelocityParams.Mode = FinishVelocityMode;
			ConstantForce->FinishVelocityParams.SetVelocity = FinishSetVelocity;
			ConstantForce->FinishVelocityParams.ClampVelocity = FinishClampVelocity;
			ConstantForce->Settings.SetFlag(ERootMotionSourceSettingsFlags::IgnoreZAccumulate);

			RootMotionSourceID = MovementComponent->ApplyRootMotionSource(ConstantForce);
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_ARM_MoveToForce called in Ability %s with null MovementComponent; Task Instance Name %s."),
			Ability ? *Ability->GetName() : TEXT("NULL"),
			*InstanceName.ToString());
	}
}

void UAbilityTask_ARM_MoveToForce::TickTask(float DeltaTime)
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
					OnFinished.ExecuteIfBound();
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

void UAbilityTask_ARM_MoveToForce::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAbilityTask_ARM_MoveToForce, StartLocation);
	DOREPLIFETIME(UAbilityTask_ARM_MoveToForce, TargetLocation);
	DOREPLIFETIME(UAbilityTask_ARM_MoveToForce, Duration);
}

void UAbilityTask_ARM_MoveToForce::PreDestroyFromReplication()
{
	bIsFinished = true;
	EndTask();
}

void UAbilityTask_ARM_MoveToForce::OnDestroy(bool AbilityIsEnding)
{
	if (MovementComponent)
	{
		MovementComponent->RemoveRootMotionSourceByID(RootMotionSourceID);
	}

	Super::OnDestroy(AbilityIsEnding);
}

