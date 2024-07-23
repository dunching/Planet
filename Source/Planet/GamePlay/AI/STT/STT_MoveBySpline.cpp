
#include "STT_MoveBySpline.h"

#include <NavigationSystem.h>
#include <Tasks/AITask_MoveTo.h>
#include <Components/SplineComponent.h>

#include "AITask_MoveBySpline.h"
#include "HumanAIController.h"
#include "HumanCharacter.h"

#ifdef WITH_EDITOR
static TAutoConsoleVariable<int32> DrawDebugSTT_MoveBySpline(
	TEXT("Skill.DrawDebug.STT_MoveBySpline"),
	0,
	TEXT("")
	TEXT(" default: 0"));
#endif

EStateTreeRunStatus FSTT_MoveBySpline::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
)const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.CharacterPtr)
	{
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(InstanceData.AIControllerPtr->FindComponentByInterface(UGameplayTaskOwnerInterface::StaticClass()));
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.AIControllerPtr;
	}

	InstanceData.PointIndex = 0;
	return PerformMoveTask(Context, *InstanceData.AIControllerPtr); 
}

void FSTT_MoveBySpline::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
)const
{
	Super::ExitState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.AITaskPtr && InstanceData.AITaskPtr->GetState() != EGameplayTaskState::Finished)
	{
		InstanceData.AITaskPtr->ExternalCancel();
	}
	InstanceData.AITaskPtr = nullptr;
}

EStateTreeRunStatus FSTT_MoveBySpline::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.AITaskPtr)
	{
		if (InstanceData.AITaskPtr->GetState() == EGameplayTaskState::Finished)
		{
			if (InstanceData.AITaskPtr->WasMoveSuccessful())
			{
				if (InstanceData.PointIndex < InstanceData.SPlinePtr->GetNumberOfSplinePoints())
				{
					return PerformMoveTask(Context, *InstanceData.AIControllerPtr);
				}
				else
				{
					return EStateTreeRunStatus::Succeeded;
				}
			}
			else
			{
				return EStateTreeRunStatus::Failed;
			}
		}

		return Super::Tick(Context, DeltaTime);
	}
	return EStateTreeRunStatus::Failed;
}

FSTT_MoveBySpline::FAITaskType* FSTT_MoveBySpline::PrepareMoveToTask(
	FStateTreeExecutionContext& Context,
	AAIController& Controller, 
	FAITaskType* ExistingTask, 
	FAIMoveRequest& MoveRequest) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	auto MoveTask = ExistingTask ? ExistingTask : UAITask::NewAITask<UAITask_MoveBySpline>(Controller, *InstanceData.TaskOwner);
	if (MoveTask)
	{
		MoveTask->SetUp(&Controller, MoveRequest);
	}

	return MoveTask;
}

EStateTreeRunStatus FSTT_MoveBySpline::PerformMoveTask(
	FStateTreeExecutionContext& Context,
	AAIController& Controller
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	FAIMoveRequest MoveReq;
	MoveReq.SetNavigationFilter(InstanceData.FilterClass ? InstanceData.FilterClass : Controller.GetDefaultNavigationFilterClass())
		.SetAllowPartialPath(InstanceData.bAllowPartialPath)
		.SetAcceptanceRadius(InstanceData.AcceptableRadius)
		.SetCanStrafe(InstanceData.bAllowStrafe)
		.SetReachTestIncludesAgentRadius(InstanceData.bReachTestIncludesAgentRadius)
		.SetReachTestIncludesGoalRadius(InstanceData.bReachTestIncludesGoalRadius)
		.SetRequireNavigableEndLocation(InstanceData.bRequireNavigableEndLocation)
		.SetProjectGoalLocation(InstanceData.bProjectGoalLocation)
		.SetUsePathfinding(true);

	const auto SPlinePoint = InstanceData.SPlinePtr->GetWorldLocationAtSplinePoint(InstanceData.PointIndex);
	InstanceData.PointIndex++;

#ifdef WITH_EDITOR
	if (DrawDebugSTT_MoveBySpline.GetValueOnGameThread())
	{
		DrawDebugSphere(InstanceData.SPlinePtr->GetWorld(), SPlinePoint, 20, 20, FColor::Yellow, false, 5);
	}
#endif

	MoveReq.SetGoalLocation(SPlinePoint);

	if (MoveReq.IsValid())
	{
		InstanceData.AITaskPtr = nullptr;
		InstanceData.AITaskPtr = PrepareMoveToTask(Context, Controller, InstanceData.AITaskPtr, MoveReq);
		if (InstanceData.AITaskPtr)
		{
			if (
				InstanceData.AITaskPtr->IsActive() || 
				InstanceData.AITaskPtr->IsFinished()
				)
			{
				InstanceData.AITaskPtr->ConditionalPerformMove();
			}
			else
			{
				InstanceData.AITaskPtr->ReadyForActivation();
			}

			if (InstanceData.AITaskPtr->GetState() == EGameplayTaskState::Finished)
			{
				return InstanceData.AITaskPtr->WasMoveSuccessful() ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Failed;
			}

			return EStateTreeRunStatus::Running;
		}
	}

	UE_VLOG(Context.GetOwner(), LogStateTree, Error, TEXT("FStateTreeMoveToTask failed because it doesn't have a destination."));
	return EStateTreeRunStatus::Failed;
}
