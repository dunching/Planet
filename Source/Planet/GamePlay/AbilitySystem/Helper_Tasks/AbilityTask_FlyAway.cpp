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
#include "GameplayTagsSubSystem.h"

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
	MyTask->RootMotionAccumulateMode = ERootMotionAccumulateMode::Override;
	MyTask->FinishSetVelocity = FVector::ZeroVector;
	MyTask->FinishClampVelocity = 0.f;

	MyTask->Duration = InDuration;
	MyTask->Height = InHeight;

	return MyTask;
}

UAbilityTask_FlyAway* UAbilityTask_FlyAway::NewTask(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	ERootMotionAccumulateMode RootMotionAccumulateMode,
	float InDuration,
	float InHeight
)
{
	UAbilityTask_FlyAway* MyTask = NewAbilityTask<UAbilityTask_FlyAway>(OwningAbility);

	MyTask->ForceName = TaskInstanceName;
	MyTask->FinishVelocityMode = ERootMotionFinishVelocityMode::SetVelocity;
	MyTask->RootMotionAccumulateMode = RootMotionAccumulateMode;
	MyTask->FinishSetVelocity = FVector::ZeroVector;
	MyTask->FinishClampVelocity = 0.f;

	MyTask->Duration = InDuration;
	MyTask->Height = InHeight;

	return MyTask;
}

void UAbilityTask_FlyAway::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	const bool bTimedOut = HasTimedOut();
	if (bTimedOut)
	{
		bIsFinished = true;
		OnFinish.ExecuteIfBound();
		EndTask();
	}
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
		ASC->AddLooseGameplayTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_IntoFly);

		MovementComponent = Cast<UCharacterMovementComponent>(ASC->AbilityActorInfo->MovementComponent.Get());
		if (MovementComponent)
		{
			ForceName = ForceName.IsNone() ? FName("AbilityTaskApplyRootMotionConstantForce") : ForceName;
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
				ASC->AbilityActorInfo->AvatarActor->GetActorLocation(),
				Cast<ACharacter>(ASC->AbilityActorInfo->AvatarActor)
			);

			RootMotionSourceID = MovementComponent->ApplyRootMotionSource(RootMotionSourceSPtr);
		}
	}
}

void UAbilityTask_FlyAway::OnDestroy(bool AbilityIsEnding)
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC && ASC->AbilityActorInfo->MovementComponent.IsValid())
	{
		ASC->RemoveLooseGameplayTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_IntoFly);
	}

	if (MovementComponent)
	{
		MovementComponent->RemoveRootMotionSourceByID(RootMotionSourceID);
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UAbilityTask_FlyAway::UpdateDuration()
{
	if (MovementComponent)
	{
		auto Ptr = MovementComponent->GetRootMotionSourceByID(RootMotionSourceID);
		TSharedPtr<FRootMotionSource_FlyAway>RootMotionSourceSPtr(Ptr, dynamic_cast<FRootMotionSource_FlyAway*>(Ptr.Get()));

		RootMotionSourceSPtr->UpdateDuration(Height, Duration, MovementComponent->GetActorLocation());
	}
}

void UAbilityTask_FlyAway::UpdateDuration(int32 InHeight, float InDuration)
{
	Height = InHeight;
	Duration = InDuration;

	UpdateDuration();
}

void UAbilityTask_FlyAway::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Height);
	DOREPLIFETIME(ThisClass, Duration);
	DOREPLIFETIME(ThisClass, RootMotionAccumulateMode);
}
