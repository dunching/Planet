#include "AbilityTask_ARM_Tornado.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/RootMotionSource.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "Components/CapsuleComponent.h"

#include "Helper_RootMotionSource.h"
#include "CharacterBase.h"
#include "CollisionDataStruct.h"
#include "HumanCharacter.h"
#include "Skill_Active_Tornado.h"
#include "Skill_Active_Traction.h"
#include "SPlineActor.h"
#include "Tornado.h"
#include "TractionActor.h"

UAbilityTask_ARM_Tornado::UAbilityTask_ARM_Tornado(
	const FObjectInitializer& ObjectInitializer
) :
  Super(ObjectInitializer)
{
}

void UAbilityTask_ARM_Tornado::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, TornadoPtr, COND_InitialOnly);
}

UAbilityTask_ARM_Tornado* UAbilityTask_ARM_Tornado::ApplyRootMotionTornado(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	TWeakObjectPtr<ATornado> TornadoPtr
)
{
	auto MyTask = NewAbilityTask<UAbilityTask_ARM_Tornado>(OwningAbility, TaskInstanceName);

	MyTask->TornadoPtr = TornadoPtr;

	return MyTask;
}

void UAbilityTask_ARM_Tornado::SharedInitAndApply()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC && ASC->AbilityActorInfo->MovementComponent.IsValid())
	{
		MovementComponent = Cast<UCharacterMovementComponent>(ASC->AbilityActorInfo->MovementComponent.Get());

		if (MovementComponent && TornadoPtr.IsValid())
		{
			ForceName = ForceName.IsNone() ? FName("AbilityTaskApplyRootMotionRadialForce") : ForceName;
			auto RootMotionSourceSPtr = MakeShared<
				FRootMotionSource_ByTornado>();
					
			RootMotionSourceSPtr->AccumulateMode = ERootMotionAccumulateMode::Override;
			RootMotionSourceSPtr->Priority = ERootMotionSource_Priority::kTraction;
			RootMotionSourceSPtr->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::SetVelocity;
			RootMotionSourceSPtr->FinishVelocityParams.SetVelocity = FVector::ZeroVector;
			RootMotionSourceSPtr->FinishVelocityParams.ClampVelocity = 0.f;

			// 直接取出来
			RootMotionSourceSPtr->InnerRadius = TornadoPtr->ItemProxy_DescriptionPtr->InnerRadius;
			RootMotionSourceSPtr->MaxHeight = TornadoPtr->ItemProxy_DescriptionPtr->MaxHeight;
			RootMotionSourceSPtr->RotationSpeed = TornadoPtr->ItemProxy_DescriptionPtr->RotationSpeed;
			RootMotionSourceSPtr->TornadoPtr = TornadoPtr;

			// TargetCharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
			// TargetCharacterMovementComponent->MaxFlySpeed = 2000.f;
			// TargetCharacterMovementComponent->BrakingDecelerationFlying = 4000.f;

			RootMotionSourceID = MovementComponent->ApplyRootMotionSource(RootMotionSourceSPtr);
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_ARM_MoveToForce called in Ability %s with null MovementComponent; Task Instance Name %s."),
			Ability ? *Ability->GetName() : TEXT("NULL"),
			*InstanceName.ToString());
	}
}

void UAbilityTask_ARM_Tornado::Activate()
{
	Super::Activate();

	SharedInitAndApply();
}

void UAbilityTask_ARM_Tornado::TickTask(
	float DeltaTime
)
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

		if (bTimedOut)
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

void UAbilityTask_ARM_Tornado::PreDestroyFromReplication()
{
	bIsFinished = true;
	EndTask();
	
	Super::PreDestroyFromReplication();
}

void UAbilityTask_ARM_Tornado::OnDestroy(
	bool AbilityIsEnding
)
{
	if (MovementComponent)
	{
		MovementComponent->RemoveRootMotionSourceByID(RootMotionSourceID);
	}

	Super::OnDestroy(AbilityIsEnding);
}
