
#include "STT_MoveByFormation.h"

#include <NavigationSystem.h>
#include <Tasks/AITask_MoveTo.h>
#include <Components/SplineComponent.h>
#include <GameFramework/CharacterMovementComponent.h>

#include "AIComponent.h"
#include "AITask_MoveBySpline.h"
#include "AS_Character.h"
#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "Helper_RootMotionSource.h"
#include "CharacterAttributesComponent.h"
#include "GravityMovementComponent.h"
#include "HumanCharacter_AI.h"

#ifdef WITH_EDITOR
static TAutoConsoleVariable<int32> DrawDebugSTT_MoveByFormation(
	TEXT("Skill.DrawDebug.STT_MoveByFormation"),
	0,
	TEXT("")
	TEXT(" default: 0"));
#endif

EStateTreeRunStatus FSTT_MoveByFormation::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
)const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.CharacterPtr)
	{
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.CharacterPtr->GetGravityMovementComponent()->bForceRotation_OrientToMovement = true;

	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(InstanceData.AIControllerPtr->FindComponentByInterface(UGameplayTaskOwnerInterface::StaticClass()));
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.AIControllerPtr;
	}

	return PerformGameplayTask(Context, *InstanceData.AIControllerPtr); 
}

void FSTT_MoveByFormation::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
)const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.CharacterPtr->GetCharacterMovement()->RemoveRootMotionSourceByID(InstanceData.RootMotionSourceID);

	InstanceData.CharacterPtr->GetGravityMovementComponent()->bForceRotation_OrientToMovement = false;

	Super::ExitState(Context, Transition);
}

EStateTreeRunStatus FSTT_MoveByFormation::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.RootMotionSourceID > 0)
	{
		return Super::Tick(Context, DeltaTime);
	}
	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus FSTT_MoveByFormation::PerformGameplayTask(
	FStateTreeExecutionContext& Context,
	AAIController& Controller
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	auto RootMotionSourcePtr = MakeShared<FRootMotionSource_Formation>();
	RootMotionSourcePtr->InstanceName = TEXT("RootMotionSource_Formation");
	RootMotionSourcePtr->AccumulateMode = ERootMotionAccumulateMode::Additive;
	RootMotionSourcePtr->Priority = ERootMotionSource_Priority::kAIMove;
	RootMotionSourcePtr->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::ClampVelocity;
	RootMotionSourcePtr->FinishVelocityParams.ClampVelocity =
		InstanceData.CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetPerformSpeed();
	RootMotionSourcePtr->Settings.SetFlag(ERootMotionSourceSettingsFlags::IgnoreZAccumulate);

	RootMotionSourcePtr->FormationPtr = InstanceData.CharacterPtr->GetAIComponent()->PathFollowComponentPtr;
	RootMotionSourcePtr->Duration = -1.f;

	InstanceData.RootMotionSourceID =
		InstanceData.CharacterPtr->GetCharacterMovement()->ApplyRootMotionSource(RootMotionSourcePtr);

	return EStateTreeRunStatus::Running;
}
