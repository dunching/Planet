
#include "STT_CheckHasTask.h"

#include "AIComponent.h"
#include "ResourceBoxStateTreeComponent.h"
#include "HumanCharacter_AI.h"

const UStruct* FSTT_CheckHasTask::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_CheckHasTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FStateTreeTaskBase::EnterState(Context, Transition);
	
	return PerformMoveTask(Context);
}

void FSTT_CheckHasTask::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FStateTreeTaskBase::ExitState(Context, Transition);
}

EStateTreeRunStatus FSTT_CheckHasTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	return FStateTreeTaskBase::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_CheckHasTask::PerformMoveTask(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.CharacterPtr->GetAIComponent()->PresetTaskNodesAry.IsEmpty())
	{
		return EStateTreeRunStatus::Succeeded;
	}
	return EStateTreeRunStatus::Failed;
}

const UStruct* FSTT_CheckHasTemopraryTask::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_CheckHasTemopraryTask::EnterState(FStateTreeExecutionContext& Context,
                                                           const FStateTreeTransitionResult& Transition) const
{
	FStateTreeTaskBase::EnterState(Context, Transition);
	
	return PerformMoveTask(Context);
}

void FSTT_CheckHasTemopraryTask::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FStateTreeTaskBase::ExitState(Context, Transition);
}

EStateTreeRunStatus FSTT_CheckHasTemopraryTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	return FStateTreeTaskBase::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_CheckHasTemopraryTask::PerformMoveTask(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.CharacterPtr->GetAIComponent()->TemporaryTaskNodesAry.IsEmpty())
	{
		return EStateTreeRunStatus::Succeeded;
	}
	return EStateTreeRunStatus::Failed;
}
