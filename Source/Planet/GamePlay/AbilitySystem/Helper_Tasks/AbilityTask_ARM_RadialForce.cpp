
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

#include "Helper_RootMotionSource.h"
#include "CharacterBase.h"
#include "CS_RootMotion_Traction.h"

UAbilityTask_ARM_RadialForce::UAbilityTask_ARM_RadialForce(
	const FObjectInitializer& ObjectInitializer
)
	: Super(ObjectInitializer)
{
}

void UAbilityTask_ARM_RadialForce::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TractionPointPtr);
}

UAbilityTask_ARM_RadialForce* UAbilityTask_ARM_RadialForce::MyApplyRootMotionRadialForce(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName, 
	TWeakObjectPtr<ATractionPoint>InTractionPointPtr
)
{
	auto MyTask = NewAbilityTask<UAbilityTask_ARM_RadialForce>(OwningAbility, TaskInstanceName);

	MyTask->ForceName = TaskInstanceName;

	MyTask->Location = InTractionPointPtr->GetActorLocation();
	MyTask->Strength = InTractionPointPtr->Strength;
	MyTask->Radius = FMath::Max(InTractionPointPtr->Radius, SMALL_NUMBER); // No zero radius
	MyTask->Duration = InTractionPointPtr->PrimaryActorTick.TickInterval;
	MyTask->bIsPush = InTractionPointPtr->bIsPush;
	MyTask->bIsAdditive = InTractionPointPtr->bIsAdditive;
	MyTask->bNoZForce = InTractionPointPtr->bNoZForce;
	MyTask->StrengthDistanceFalloff = InTractionPointPtr->StrengthDistanceFalloff;
	MyTask->StrengthOverTime = InTractionPointPtr->StrengthOverTime;
	MyTask->bUseFixedWorldDirection = InTractionPointPtr->bUseFixedWorldDirection;
	MyTask->FixedWorldDirection = InTractionPointPtr->FixedWorldDirection;
	MyTask->FinishVelocityMode = InTractionPointPtr->VelocityOnFinishMode;
	MyTask->FinishSetVelocity = InTractionPointPtr->SetVelocityOnFinish;
	MyTask->FinishClampVelocity = InTractionPointPtr->ClampVelocityOnFinished;

	MyTask->TractionPointPtr = InTractionPointPtr;

	MyTask->SharedInitAndApply();

	return MyTask;
}

void UAbilityTask_ARM_RadialForce::SharedInitAndApply()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC && ASC->AbilityActorInfo->MovementComponent.IsValid())
	{
		MovementComponent = Cast<UCharacterMovementComponent>(ASC->AbilityActorInfo->MovementComponent.Get());
		StartTime = GetWorld()->GetTimeSeconds();
		EndTime = StartTime + Duration;

		if (MovementComponent)
		{
			ForceName = ForceName.IsNone() ? FName("AbilityTaskApplyRootMotionRadialForce") : ForceName;
			TSharedPtr<FRootMotionSource_MyRadialForce> RadialForce = MakeShared<FRootMotionSource_MyRadialForce>();
			RadialForce->InstanceName = ForceName;
			RadialForce->AccumulateMode = bIsAdditive ? ERootMotionAccumulateMode::Additive : ERootMotionAccumulateMode::Override;
			RadialForce->Priority = ERootMotionSource_Priority::kTraction;
			RadialForce->Location = Location;
			RadialForce->Duration = -1.f;
			RadialForce->Radius = Radius;
			RadialForce->Strength = Strength;
			RadialForce->bIsPush = bIsPush;
			RadialForce->bNoZForce = bNoZForce;
			RadialForce->StrengthDistanceFalloff = StrengthDistanceFalloff;
			RadialForce->StrengthOverTime = StrengthOverTime;
			RadialForce->bUseFixedWorldDirection = bUseFixedWorldDirection;
			RadialForce->FixedWorldDirection = FixedWorldDirection;
			RadialForce->FinishVelocityParams.Mode = FinishVelocityMode;
			RadialForce->FinishVelocityParams.SetVelocity = FinishSetVelocity;
			RadialForce->FinishVelocityParams.ClampVelocity = FinishClampVelocity;

			RadialForce->TractionPointPtr = TractionPointPtr;

			RootMotionSourceID = MovementComponent->ApplyRootMotionSource(RadialForce);
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_ApplyRootMotionRadialForce called in Ability %s with null MovementComponent; Task Instance Name %s."),
			Ability ? *Ability->GetName() : TEXT("NULL"),
			*InstanceName.ToString());
	}
}

void UAbilityTask_ARM_RadialForce::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	const bool bTimedOut = HasTimedOut();
	const auto Distance = FVector::Distance(Location, MovementComponent->GetActorLocation());
	if (
		bTimedOut ||
		(Distance > Radius)
		)
	{
		bIsFinished = true;
		OnFinish.ExecuteIfBound();
		EndTask();
	}
	else
	{
	}
}

void UAbilityTask_ARM_RadialForce::OnDestroy(bool AbilityIsEnding)
{
	if (MovementComponent)
	{
		MovementComponent->RemoveRootMotionSourceByID(RootMotionSourceID);
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UAbilityTask_ARM_RadialForce::UpdateLocation(TWeakObjectPtr<ATractionPoint>InTractionPointPtr)
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
					RootMotionSourceSPtr->LocationActor = InTractionPointPtr.Get();
				}
			}
		}
	}
}
