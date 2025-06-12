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
#include "Skill_Active_Traction.h"
#include "SPlineActor.h"
#include "TractionActor.h"

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
}

UAbilityTask_ARM_RadialForce* UAbilityTask_ARM_RadialForce::ApplyRootMotionRadialForce(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	TWeakObjectPtr<ATractionPoint> TractionPoinAcotrPtr
)
{
	auto MyTask = NewAbilityTask<UAbilityTask_ARM_RadialForce>(OwningAbility, TaskInstanceName);

	MyTask->TractionPoinAcotrPtr = TractionPoinAcotrPtr;

	return MyTask;
}

void UAbilityTask_ARM_RadialForce::SharedInitAndApply()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC && ASC->AbilityActorInfo->MovementComponent.IsValid())
	{
		MovementComponent = Cast<UCharacterMovementComponent>(ASC->AbilityActorInfo->MovementComponent.Get());

		if (MovementComponent.IsValid() && TractionPoinAcotrPtr.IsValid())
		{
			ForceName = ForceName.IsNone() ? FName("AbilityTaskApplyRootMotionRadialForce") : ForceName;
			auto RadialForce = MakeShared<
				FRootMotionSource_MyRadialForce>();
					
			RadialForce->InstanceName = ForceName;
					
			RadialForce->AccumulateMode = ERootMotionAccumulateMode::Additive;
			RadialForce->Priority = ERootMotionSource_Priority::kTraction;
			RadialForce->FinishVelocityParams.Mode = FinishVelocityMode;
			RadialForce->FinishVelocityParams.SetVelocity = FinishSetVelocity;
			RadialForce->FinishVelocityParams.ClampVelocity = FinishClampVelocity;
			
			RadialForce->Radius = TractionPoinAcotrPtr->ItemProxy_DescriptionPtr->OuterRadius;
			RadialForce->bNoZForce = true;
			RadialForce->Strength = TractionPoinAcotrPtr->ItemProxy_DescriptionPtr->Strength;
			RadialForce->bIsPush = TractionPoinAcotrPtr->ItemProxy_DescriptionPtr->bIsPush;
			RadialForce->StrengthDistanceFalloff = TractionPoinAcotrPtr->ItemProxy_DescriptionPtr->StrengthDistanceFalloff.LoadSynchronous();

			RadialForce->TractionPoinAcotrPtr = TractionPoinAcotrPtr;
			
			RootMotionSourceID = MovementComponent->ApplyRootMotionSource(RadialForce);
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_ARM_MoveToForce called in Ability %s with null MovementComponent; Task Instance Name %s."),
			Ability ? *Ability->GetName() : TEXT("NULL"),
			*InstanceName.ToString());
	}
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

void UAbilityTask_ARM_RadialForce::PreDestroyFromReplication()
{
	bIsFinished = true;
	EndTask();
	
	Super::PreDestroyFromReplication();
}

void UAbilityTask_ARM_RadialForce::OnDestroy(
	bool AbilityIsEnding
)
{
	if (MovementComponent.IsValid())
	{
		MovementComponent->RemoveRootMotionSourceByID(RootMotionSourceID);
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UAbilityTask_ARM_RadialForce::UpdateLocation(
	TWeakObjectPtr<ATractionPoint> InTractionPointPtr
)
{
	if (MovementComponent.IsValid())
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
