
#include "STT_RotateToFaceEntry.h"

#include <NavigationSystem.h>
#include <EnvironmentQuery/EnvQueryManager.h>

#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "AITask_ReleaseSkill.h"
#include "STE_Human.h"
#include "Planet.h"
#include "PlanetPlayerController.h"

namespace STT_RotateToFaceEntry
{
	FName Donut_InnerRadius = TEXT("Donut.InnerRadius");

	FName Donut_OuterRadius = TEXT("Donut.OuterRadius");
}

FSTT_RotateToFaceEntry::FSTT_RotateToFaceEntry() :
	Super()
{
}

const UStruct* FSTT_RotateToFaceEntry::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_RotateToFaceEntry::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
)const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.CharacterPtr)
	{
		return EStateTreeRunStatus::Failed;
	}

	return PerformMoveTask(Context);
}

void FSTT_RotateToFaceEntry::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
)const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.AIControllerPtr)
	{
		InstanceData.AIControllerPtr->ClearFocus(EAIFocusPriority::Gameplay);
	}

	Super::ExitState(Context, Transition);
}

EStateTreeRunStatus FSTT_RotateToFaceEntry::Tick( 
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.AIControllerPtr->GetFocusActor())
	{
		const float AngleTolerance = 5.f;

		const auto PCRot = InstanceData.AIControllerPtr->GetControlRotation();
		const auto PawnRot = InstanceData.CharacterPtr->GetActorRotation();

		// Gravity?
		if (PCRot.EqualsOrientation(PawnRot, AngleTolerance))
		{
			return EStateTreeRunStatus::Succeeded;
		}
	}
	else
	{
		return EStateTreeRunStatus::Failed;
	}

	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_RotateToFaceEntry::PerformMoveTask(FStateTreeExecutionContext& Context)const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.CharacterPtr && InstanceData.AIControllerPtr)
	{
		check(InstanceData.CharacterPtr != InstanceData.TargetCharacterPtr);
		InstanceData.AIControllerPtr->SetFocus(InstanceData.TargetCharacterPtr.Get());

		return EStateTreeRunStatus::Running;
	}
	return EStateTreeRunStatus::Failed;
}