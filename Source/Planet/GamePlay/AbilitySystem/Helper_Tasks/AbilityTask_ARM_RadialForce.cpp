#include "AbilityTask_ARM_RadialForce.h"

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
#include "SPlineActor.h"

UAbilityTask_ARM_RadialForce::UAbilityTask_ARM_RadialForce(
	const FObjectInitializer& ObjectInitializer
) :
  Super(ObjectInitializer)
{
}

void UAbilityTask_ARM_RadialForce::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, TractionPoinAcotrPtr, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, Radius, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, InnerRadius, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, Strength, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, Duration, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, bIsPush, COND_InitialOnly);
}

UAbilityTask_ARM_RadialForce* UAbilityTask_ARM_RadialForce::MyApplyRootMotionRadialForce(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	TWeakObjectPtr<ATractionPoint> TractionPoinAcotrPtr,
	float Strength,
	float Duration,
	float Radius,
	float InnerRadius,
	bool bIsPush
)
{
	auto MyTask = NewAbilityTask<UAbilityTask_ARM_RadialForce>(OwningAbility, TaskInstanceName);

	MyTask->Strength = Strength;
	MyTask->Duration = Duration;
	MyTask->Radius = Radius;
	MyTask->InnerRadius = InnerRadius;
	MyTask->bIsPush = bIsPush;
	MyTask->TractionPoinAcotrPtr = TractionPoinAcotrPtr;

	return MyTask;
}

void UAbilityTask_ARM_RadialForce::SharedInitAndApply()
{
	StartTime = GetWorld()->GetTimeSeconds();
	CurrentTime = GetWorld()->GetTimeSeconds();

	EndTime = StartTime + Duration;

	UpdateTarget();
}

void UAbilityTask_ARM_RadialForce::Activate()
{
	Super::Activate();

	SharedInitAndApply();
}

void UAbilityTask_ARM_RadialForce::TickTask(
	float DeltaTime
)
{
	Super::TickTask(DeltaTime);

	const bool bTimedOut = HasTimedOut();
	if (GetWorld()->GetTimeSeconds() > EndTime)
	{
		bIsFinished = true;
		OnFinish.ExecuteIfBound();
		EndTask();
	}
	else
	{
		CurrentTime += DeltaTime;
		IntervalTime += DeltaTime;
		if (IntervalTime >= Interval)
		{
			IntervalTime = 0.f;
			UpdateTarget();
		}
	}
}

void UAbilityTask_ARM_RadialForce::OnDestroy(
	bool AbilityIsEnding
)
{
	if (MovementComponent)
	{
		MovementComponent->RemoveRootMotionSourceByID(RootMotionSourceID);
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UAbilityTask_ARM_RadialForce::UpdateLocation(
	TWeakObjectPtr<ATractionPoint> InTractionPointPtr
)
{
	if (MovementComponent)
	{
		TSharedPtr<FRootMotionSource> RMS = MovementComponent->GetRootMotionSourceByID(RootMotionSourceID);
		if (RMS.IsValid())
		{
			if (RMS->GetScriptStruct() == FRootMotionSource_RadialForce::StaticStruct())
			{
				auto RootMotionSourceSPtr = static_cast<FRootMotionSource_MyRadialForce*>(RMS.Get());
				if (RootMotionSourceSPtr)
				{
				}
			}
		}
	}
}

void UAbilityTask_ARM_RadialForce::UpdateTarget()
{
	TArray<FOverlapResult> OutOverlaps;
	FCollisionObjectQueryParams ObjectQueryParams;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetAvatarActor());

	ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);
	GetWorld()->OverlapMultiByObjectType(
		OutOverlaps,
		TractionPoinAcotrPtr->GetActorLocation(),
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	for (const auto& Iter : OutOverlaps)
	{
		auto TargetCharacter = Cast<AHumanCharacter>(Iter.GetActor());
		if (TargetCharacter)
		{
			auto TargetCharacterMovementComponent = TargetCharacter->GetCharacterMovement();
			if (TargetCharacterMovementComponent->HasRootMotionSources())
			{
			}
			else
			{
				if (TargetCharacterMovementComponent)
				{
					float OutRadius = 0.f;
					float OutHalfHeight = 0.f;
					TargetCharacter->GetCapsuleComponent()->GetScaledCapsuleSize(OutRadius, OutHalfHeight);

					ForceName = ForceName.IsNone() ? FName("AbilityTaskApplyRootMotionRadialForce") : ForceName;
					TSharedPtr<FRootMotionSource_MyRadialForce> RadialForce = MakeShared<
						FRootMotionSource_MyRadialForce>();
					
					RadialForce->InstanceName = ForceName;
					
					RadialForce->AccumulateMode = ERootMotionAccumulateMode::Additive;
					RadialForce->Priority = ERootMotionSource_Priority::kTraction;
					RadialForce->FinishVelocityParams.Mode = FinishVelocityMode;
					RadialForce->FinishVelocityParams.SetVelocity = FinishSetVelocity;
					RadialForce->FinishVelocityParams.ClampVelocity = FinishClampVelocity;
					
					RadialForce->TractionPoinAcotrPtr = TractionPoinAcotrPtr;
					RadialForce->Duration = EndTime - CurrentTime;
					RadialForce->Radius = Radius;
					RadialForce->InnerRadius = InnerRadius + OutRadius;
					RadialForce->Strength = Strength;
					RadialForce->bIsPush = bIsPush;
					RadialForce->bNoZForce = true;

					TargetCharacterMovementComponent->ApplyRootMotionSource(RadialForce);
				}
			}
		}
	}
}
