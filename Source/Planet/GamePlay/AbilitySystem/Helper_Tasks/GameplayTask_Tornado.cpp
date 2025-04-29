// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayTask_Tornado.h"

#include "CharacterAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include "CharacterBase.h"
#include "Helper_RootMotionSource.h"
#include "Skill_Active_Tornado.h"
#include "Tornado.h"
#include "Math/UnitConversion.h"

UGameplayTask_Tornado::UGameplayTask_Tornado(
	const FObjectInitializer& ObjectInitializer
) :
  Super(ObjectInitializer)
{
	bTickingTask = true;
	bSimulatedTask = true;
}

void UGameplayTask_Tornado::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, TornadoPtr, COND_InitialOnly);
}

void UGameplayTask_Tornado::Activate()
{
	Super::Activate();

	PerformAction();
}

void UGameplayTask_Tornado::TickTask(
	float DeltaTime
)
{
	Super::TickTask(DeltaTime);

	if (TornadoPtr.IsValid())
	{
	}
	else
	{
		OnFinish.ExecuteIfBound();
		EndTask();
	}
}

void UGameplayTask_Tornado::OnDestroy(
	bool AbilityIsEnding
)
{
	auto TargetCharacterPtr = Cast<ACharacterBase>(GetAvatarActor());
	if (!TargetCharacterPtr)
	{
		return;
	}

	if (TargetCharacterPtr->GetCharacterMovement())
	{
		TargetCharacterPtr->GetCharacterMovement()->RemoveRootMotionSourceByID(RootMotionSourceID);
	}

	Super::OnDestroy(AbilityIsEnding);
}

inline void UGameplayTask_Tornado::InitSimulatedTask(
	UGameplayTasksComponent& InGameplayTasksComponent
)
{
	Super::InitSimulatedTask(InGameplayTasksComponent);

	PerformAction();
}

void UGameplayTask_Tornado::OnRep_ReplicatedUsing()
{
}

void UGameplayTask_Tornado::PerformAction()
{
	auto TargetCharacterPtr = Cast<ACharacterBase>(GetAvatarActor());
	if (!TargetCharacterPtr)
	{
		return;
	}

	//
	FGameplayTagContainer TagContainer;
	if (TargetCharacterPtr->GetCharacterAbilitySystemComponent()->HasAnyMatchingGameplayTags(TagContainer))
	{
		EndTask();
		return;
	}

	auto TargetCharacterMovementComponent = TargetCharacterPtr->GetCharacterMovement();
	if (TargetCharacterMovementComponent->HasRootMotionSources())
	{
	}
	else
	{
		if (TargetCharacterMovementComponent)
		{
			float OutRadius = 0.f;
			float OutHalfHeight = 0.f;
			TargetCharacterPtr->GetCapsuleComponent()->GetScaledCapsuleSize(OutRadius, OutHalfHeight);

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

			RootMotionSourceID = TargetCharacterMovementComponent->ApplyRootMotionSource(RootMotionSourceSPtr);
		}
	}
}
