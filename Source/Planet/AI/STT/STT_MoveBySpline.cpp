
#include "STT_MoveBySpline.h"

#include <NavigationSystem.h>
#include <Tasks/AITask_MoveTo.h>
#include <Components/SplineComponent.h>

#include "AITask_MoveBySpline.h"
#include "GeneratorColony.h"
#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "STE_AICharacterController.h"
#include "GeneratorNPCs_Patrol.h"

#ifdef WITH_EDITOR
static TAutoConsoleVariable<int32> DrawDebugSTT_MoveBySpline(
	TEXT("Skill.DrawDebug.STT_MoveBySpline"),
	0,
	TEXT("")
	TEXT(" default: 0"));
#endif

EStateTreeRunStatus FSTT_CheckTarget_Spline::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.CharacterPtr)
	{
		return EStateTreeRunStatus::Failed;
	}

	return PerformAction(Context);
}

EStateTreeRunStatus FSTT_CheckTarget_Spline::PerformAction(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	if (InstanceData.AIControllerPtr->GeneratorNPCs_PatrolPtr)
	{
		if (InstanceData.GloabVariable->TargetCharacterPtr.IsValid())
		{
			const auto Value  =InstanceData.AIControllerPtr->GeneratorNPCs_PatrolPtr->CheckIsFarawayOriginal(
					InstanceData.GloabVariable->TargetCharacterPtr.Get()
				) ;
			if (Value)
			{
				// 里巡逻路线太原，我们就不需要追踪目标敌人了
				return EStateTreeRunStatus::Running;
			}
			else
			{
				InstanceData.bIsEntryAttackTarget = true;
				return EStateTreeRunStatus::Failed;
			}
		}
		else
		{
			return EStateTreeRunStatus::Running;
		}
	}
	else
	{
		return EStateTreeRunStatus::Failed;
	}
}

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

	if (!InstanceData.SPlinePtr)
	{
		return EStateTreeRunStatus::Failed;
	}
	
	return PerformGameplayTask(Context, *InstanceData.AIControllerPtr);
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
				return EStateTreeRunStatus::Succeeded;
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
		const auto Length = InstanceData.SPlinePtr->GetSplineLength();
		float CurrentLength = 0.f;
		for (;;)
		{
			const auto Pt = InstanceData.SPlinePtr->GetLocationAtDistanceAlongSpline(CurrentLength, ESplineCoordinateSpace::World);
			MoveTask->Pts.Add(Pt);

			CurrentLength += InstanceData.StepLength;
			if (CurrentLength > Length)
			{
				break;
			}
		}

		MoveTask->Index = 0;
		MoveTask->SetUp(&Controller, MoveRequest);
	}

	return MoveTask;
}

EStateTreeRunStatus FSTT_MoveBySpline::PerformGameplayTask(
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

	const auto Pt = InstanceData.SPlinePtr->GetLocationAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
	MoveReq.SetGoalLocation(Pt);

	// FNavLocation OutLocation;
	//auto NavSysPtr = UNavigationSystemV1::GetNavigationSystem(&Controller);
	//ANavigationData* UseNavData = NavSysPtr->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
	//if (NavSysPtr->ProjectPointToNavigation(
	//	SPlinePoint,
	//	OutLocation,
	//	INVALID_NAVEXTENT,
	//	UseNavData
	//))
	//{
	//}

#ifdef WITH_EDITOR
	if (DrawDebugSTT_MoveBySpline.GetValueOnGameThread())
	{
	}
#endif

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
				//				return InstanceData.AITaskPtr->WasMoveSuccessful() ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Failed;
			}

			return EStateTreeRunStatus::Running;
		}
	}

	UE_VLOG(Context.GetOwner(), LogStateTree, Error, TEXT("FStateTreeMoveToTask failed because it doesn't have a destination."));
	return EStateTreeRunStatus::Failed;
}

// void FSTT_MoveBySpline::MoveTaskCompletedSignature(TEnumAsByte<EPathFollowingResult::Type> Result)const
// {
// 	switch (Result)
// 	{
// 	case EPathFollowingResult::Success:
// 	{
// 
// 	}
// 	break;

// 	}
// }
