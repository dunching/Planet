// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilityTask_ApplyRootMotion_FlyAway.h"

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
#include "GameplayTagsLibrary.h"
#include "HumanCharacter.h"
#include "KismetGravityLibrary.h"

UAbilityTask_ApplyRootMotion_FlyAway::UAbilityTask_ApplyRootMotion_FlyAway(
	const FObjectInitializer& ObjectInitializer
) :
  Super(ObjectInitializer)
{
	bTickingTask = true;
}

UAbilityTask_ApplyRootMotion_FlyAway* UAbilityTask_ApplyRootMotion_FlyAway::NewTask(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	float InDuration,
	float InHeight
)
{
	UAbilityTask_ApplyRootMotion_FlyAway* MyTask = NewAbilityTask<UAbilityTask_ApplyRootMotion_FlyAway>(OwningAbility);

	MyTask->ForceName = TaskInstanceName;
	MyTask->FinishVelocityMode = ERootMotionFinishVelocityMode::SetVelocity;
	MyTask->RootMotionAccumulateMode = ERootMotionAccumulateMode::Override;
	MyTask->FinishSetVelocity = FVector::ZeroVector;
	MyTask->FinishClampVelocity = 0.f;

	MyTask->Duration = InDuration;
	MyTask->Height = InHeight;

	return MyTask;
}

UAbilityTask_ApplyRootMotion_FlyAway* UAbilityTask_ApplyRootMotion_FlyAway::NewTask(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	ERootMotionAccumulateMode RootMotionAccumulateMode,
	float InDuration,
	float InHeight
)
{
	UAbilityTask_ApplyRootMotion_FlyAway* MyTask = NewAbilityTask<UAbilityTask_ApplyRootMotion_FlyAway>(OwningAbility);

	MyTask->ForceName = TaskInstanceName;
	MyTask->FinishVelocityMode = ERootMotionFinishVelocityMode::SetVelocity;
	MyTask->RootMotionAccumulateMode = RootMotionAccumulateMode;
	MyTask->FinishSetVelocity = FVector::ZeroVector;
	MyTask->FinishClampVelocity = 0.f;

	MyTask->Duration = InDuration;
	MyTask->Height = InHeight;

	return MyTask;
}

UAbilityTask_ApplyRootMotion_FlyAway* UAbilityTask_ApplyRootMotion_FlyAway::NewTask(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	ERootMotionAccumulateMode RootMotionAccumulateMode,
	float InDuration,
	float InHeight,
	int32 InResingSpeed,
	int32 InFallingSpeed
)
{
	UAbilityTask_ApplyRootMotion_FlyAway* MyTask = NewAbilityTask<UAbilityTask_ApplyRootMotion_FlyAway>(OwningAbility);

	MyTask->ForceName = TaskInstanceName;
	MyTask->FinishVelocityMode = ERootMotionFinishVelocityMode::SetVelocity;
	MyTask->RootMotionAccumulateMode = RootMotionAccumulateMode;
	MyTask->FinishSetVelocity = FVector::ZeroVector;
	MyTask->FinishClampVelocity = 0.f;

	MyTask->Duration = InDuration;
	MyTask->Height = InHeight;
	MyTask->ResingSpeed = InResingSpeed;
	MyTask->FallingSpeed = InFallingSpeed;

	return MyTask;
}

void UAbilityTask_ApplyRootMotion_FlyAway::TickTask(
	float DeltaTime
)
{
	Super::TickTask(DeltaTime);

	const bool bTimedOut = HasTimedOut();
	if (bTimedOut)
	{
		bIsFinished = true;
		OnFinished.ExecuteIfBound();
		EndTask();
	}
}

void UAbilityTask_ApplyRootMotion_FlyAway::Activate()
{
	Super::Activate();

	SharedInitAndApply();
}

void UAbilityTask_ApplyRootMotion_FlyAway::SharedInitAndApply()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC && ASC->AbilityActorInfo->MovementComponent.IsValid())
	{
		ASC->AddLooseGameplayTag(UGameplayTagsLibrary::State_RootMotion_FlyAway);

		MovementComponent = Cast<UCharacterMovementComponent>(ASC->AbilityActorInfo->MovementComponent.Get());
		if (MovementComponent)
		{
			ForceName = ForceName.IsNone() ? FName("AbilityTask_ApplyRootMotion_FlyAway") : ForceName;
			auto RootMotionSourceSPtr = MakeShared<FRootMotionSource_FlyAway>();

			RootMotionSourceSPtr->InstanceName = ForceName;
			RootMotionSourceSPtr->AccumulateMode = RootMotionAccumulateMode;
			RootMotionSourceSPtr->Priority = ERootMotionSource_Priority::kFlyAway;
			RootMotionSourceSPtr->FinishVelocityParams.Mode = FinishVelocityMode;
			RootMotionSourceSPtr->FinishVelocityParams.SetVelocity = FinishSetVelocity;
			RootMotionSourceSPtr->FinishVelocityParams.ClampVelocity = FinishClampVelocity;

			RootMotionSourceSPtr->Initial(
				Height,
				Duration,
				ResingSpeed,
				FallingSpeed,
				ASC->AbilityActorInfo->AvatarActor->GetActorLocation(),
				Cast<ACharacter>(ASC->AbilityActorInfo->AvatarActor)
			);

			RootMotionSourceID = MovementComponent->ApplyRootMotionSource(RootMotionSourceSPtr);
		}
	}
}

void UAbilityTask_ApplyRootMotion_FlyAway::OnDestroy(
	bool AbilityIsEnding
)
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC && ASC->AbilityActorInfo->MovementComponent.IsValid())
	{
		ASC->RemoveLooseGameplayTag(UGameplayTagsLibrary::State_RootMotion_FlyAway);
	}

	if (MovementComponent)
	{
		MovementComponent->RemoveRootMotionSourceByID(RootMotionSourceID);
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UAbilityTask_ApplyRootMotion_FlyAway::UpdateDuration()
{
	if (MovementComponent)
	{
		auto Ptr = MovementComponent->GetRootMotionSourceByID(RootMotionSourceID);
		TSharedPtr<FRootMotionSource_FlyAway> RootMotionSourceSPtr(
			Ptr,
			dynamic_cast<FRootMotionSource_FlyAway*>(Ptr.Get())
		);

		RootMotionSourceSPtr->UpdateDuration(
			Height,
			Duration,
			ResingSpeed,
			FallingSpeed,
			MovementComponent->GetActorLocation()
		);
	}
}

void UAbilityTask_ApplyRootMotion_FlyAway::UpdateDuration(
	int32 InHeight,
	float InDuration
)
{
	Height = InHeight;
	Duration = InDuration;

	UpdateDuration();
}

void UAbilityTask_ApplyRootMotion_FlyAway::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, Height, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, Duration, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, ResingSpeed, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, FallingSpeed, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, RootMotionAccumulateMode, COND_InitialOnly);
}

UAbilityTask_HasBeenFlyAway::UAbilityTask_HasBeenFlyAway(
	const FObjectInitializer& ObjectInitializer
) :
  Super(ObjectInitializer)
{
	bTickingTask = true;
}

UAbilityTask_HasBeenFlyAway* UAbilityTask_HasBeenFlyAway::NewTask(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	int32 Height_
)
{
	auto MyTask = NewAbilityTask<UAbilityTask_HasBeenFlyAway>(OwningAbility);

	MyTask->Height = Height_;

	return MyTask;
}

void UAbilityTask_HasBeenFlyAway::Activate()
{
	Super::Activate();
}

void UAbilityTask_HasBeenFlyAway::TickTask(
	float DeltaTime
)
{
	Super::TickTask(DeltaTime);
	auto CharacterPtr = Cast<AHumanCharacter>(GetAvatarActor());
	auto CharacterMovementPtr = CharacterPtr->GetCharacterMovement();
	const FVector CurrentLocation = CharacterPtr->GetActorLocation();
	const FVector GravityDir = UKismetGravityLibrary::GetGravity(FVector::ZeroVector);

	FCollisionQueryParams Params;
	Params.bTraceComplex = false;
	Params.AddIgnoredActor(CharacterPtr);

	const ECollisionChannel CollisionChannel = CharacterMovementPtr->UpdatedComponent->GetCollisionObjectType();
	FHitResult Result;
	if (CharacterPtr->GetWorld()->LineTraceSingleByChannel(
		Result,
		CurrentLocation,
		CurrentLocation + (GravityDir * Line),
		// 避免找不到
		CollisionChannel,
		Params
	))
	{
		CurrentDistance = FMath::Abs(Result.ImpactPoint.Z - CurrentLocation.Z);
		if (!bHasBeenApex && (CurrentDistance < Height))
		{
			CharacterMovementPtr->Velocity.Z = CharacterMovementPtr->JumpZVelocity;
			CharacterMovementPtr->SetMovementMode(MOVE_Falling);
		}
		else
		{
			bHasBeenApex = true;
		}
	}
}

void UAbilityTask_HasBeenFlyAway::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Height);
	DOREPLIFETIME(ThisClass, CurrentDistance);
	DOREPLIFETIME(ThisClass, bHasBeenApex);
}
