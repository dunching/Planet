// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilityTask_Ice.h"

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

UAbilityTask_Ice::UAbilityTask_Ice(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	bTickingTask = true;
}

UAbilityTask_Ice* UAbilityTask_Ice::NewTask(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	TWeakObjectPtr<ASkill_IceGun_Projectile> InPtr,
	ACharacterBase* InTargetCharacterPtr
)
{
	UAbilityTask_Ice* MyTask = NewAbilityTask<UAbilityTask_Ice>(OwningAbility);

	MyTask->ForceName = TaskInstanceName;
	MyTask->FinishVelocityMode = ERootMotionFinishVelocityMode::SetVelocity;
	MyTask->FinishSetVelocity = FVector::ZeroVector;
	MyTask->FinishClampVelocity = 0.f;

	MyTask->IceGunPtr = InPtr;
	MyTask->TargetCharacterPtr = InTargetCharacterPtr;

	return MyTask;
}

void UAbilityTask_Ice::Activate()
{
	Super::Activate();

	SharedInitAndApply();
}

void UAbilityTask_Ice::SharedInitAndApply()
{
	if (TargetCharacterPtr && TargetCharacterPtr->GetCharacterMovement())
	{
		MovementComponent = TargetCharacterPtr->GetCharacterMovement();

		if (MovementComponent)
		{
			ForceName = ForceName.IsNone() ? FName("AbilityTaskApplyRootMotionConstantForce") : ForceName;
			TSharedPtr<FRootMotionSource_ByIce> RootMotionSource = MakeShared<FRootMotionSource_ByIce>();

			RootMotionSource->InstanceName = ForceName;
			RootMotionSource->AccumulateMode = ERootMotionAccumulateMode::Override;
			RootMotionSource->Priority = ERootMotionSource_Priority::kFlyAway;
			RootMotionSource->Duration = -1.f;
			RootMotionSource->FinishVelocityParams.Mode = FinishVelocityMode;
			RootMotionSource->FinishVelocityParams.SetVelocity = FinishSetVelocity;
			RootMotionSource->FinishVelocityParams.ClampVelocity = FinishClampVelocity;

			RootMotionSource->IceGunPtr = IceGunPtr;
			RootMotionSource->TargetCharacterPtr = TargetCharacterPtr;

			RootMotionSourceID = MovementComponent->ApplyRootMotionSource(RootMotionSource);
		}
	}
}

void UAbilityTask_Ice::OnDestroy(bool AbilityIsEnding)
{
	if (MovementComponent)
	{
		MovementComponent->RemoveRootMotionSourceByID(RootMotionSourceID);
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UAbilityTask_Ice::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	const bool bTimedOut = HasTimedOut();

	if (bTimedOut)
	{
		OnFinish.ExecuteIfBound();

		EndTask();
	}
}

