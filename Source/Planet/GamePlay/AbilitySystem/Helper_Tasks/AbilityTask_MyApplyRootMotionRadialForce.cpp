
#include "AbilityTask_MyApplyRootMotionRadialForce.h"

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

UAbilityTask_MyApplyRootMotionRadialForce::UAbilityTask_MyApplyRootMotionRadialForce(
	const FObjectInitializer& ObjectInitializer
)
	: Super(ObjectInitializer)
{
}

UAbilityTask_MyApplyRootMotionRadialForce* UAbilityTask_MyApplyRootMotionRadialForce::MyApplyRootMotionRadialForce(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName, 
	FVector Location, 
	AActor* LocationActor, 
	float Strength,
	float Duration,
	float Radius,
	bool bIsPush,
	bool bIsAdditive,
	bool bNoZForce,
	UCurveFloat* StrengthDistanceFalloff, 
	UCurveFloat* StrengthOverTime,
	bool bUseFixedWorldDirection, 
	FRotator FixedWorldDirection, 
	ERootMotionFinishVelocityMode VelocityOnFinishMode,
	FVector SetVelocityOnFinish, 
	float ClampVelocityOnFinish
)
{
	auto MyTask = NewAbilityTask<UAbilityTask_MyApplyRootMotionRadialForce>(OwningAbility, TaskInstanceName);

	MyTask->ForceName = TaskInstanceName;
	MyTask->Location = Location;
	MyTask->LocationActor = LocationActor;
	MyTask->Strength = Strength;
	MyTask->Radius = FMath::Max(Radius, SMALL_NUMBER); // No zero radius
	MyTask->Duration = Duration;
	MyTask->bIsPush = bIsPush;
	MyTask->bIsAdditive = bIsAdditive;
	MyTask->bNoZForce = bNoZForce;
	MyTask->StrengthDistanceFalloff = StrengthDistanceFalloff;
	MyTask->StrengthOverTime = StrengthOverTime;
	MyTask->bUseFixedWorldDirection = bUseFixedWorldDirection;
	MyTask->FixedWorldDirection = FixedWorldDirection;
	MyTask->FinishVelocityMode = VelocityOnFinishMode;
	MyTask->FinishSetVelocity = SetVelocityOnFinish;
	MyTask->FinishClampVelocity = ClampVelocityOnFinish;
	MyTask->SharedInitAndApply();

	return MyTask;
}

void UAbilityTask_MyApplyRootMotionRadialForce::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	const bool bTimedOut = HasTimedOut();
	const auto Distance = FVector::Distance(Location, MovementComponent->GetActorLocation());
	if (bTimedOut || (Distance > Radius))
	{
		bIsFinished = true;
		OnFinish.ExecuteIfBound();
		EndTask();
	}
}

void UAbilityTask_MyApplyRootMotionRadialForce::OnDestroy(bool AbilityIsEnding)
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

void UAbilityTask_MyApplyRootMotionRadialForce::UpdateLocation(const FVector& InLocation)
{
	if (MovementComponent)
	{
		TSharedPtr<FRootMotionSource> RMS = MovementComponent->GetRootMotionSourceByID(RootMotionSourceID);
		if (RMS.IsValid())
		{
			if (RMS->GetScriptStruct() == FRootMotionSource_RadialForce::StaticStruct())
			{
				FRootMotionSource_RadialForce* RootMotionSourceSPtr = static_cast<FRootMotionSource_RadialForce*>(RMS.Get());
				if (RootMotionSourceSPtr)
				{
					RootMotionSourceSPtr->Location = InLocation;
				}
			}
		}
	}
}
