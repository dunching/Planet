
#include "STT_Dash.h"

#include <NavigationSystem.h>

#include "AITask_DashToLeader.h"
#include "HumanAIController.h"
#include "HumanCharacter.h"

EStateTreeRunStatus FSTT_Dash::EnterState(
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

	InstanceData.AITaskPtr = PerformMoveTask(Context);

	if (InstanceData.AITaskPtr)
	{
		if (InstanceData.AITaskPtr->IsActive())
		{
			InstanceData.AITaskPtr->ConditionalPerformTask();
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

	return Super::EnterState(Context, Transition);
}

void FSTT_Dash::ExitState(
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

EStateTreeRunStatus FSTT_Dash::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.AITaskPtr)
	{
		if (InstanceData.AITaskPtr->GetState() == EGameplayTaskState::Finished)
		{
			return InstanceData.AITaskPtr->WasMoveSuccessful() ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Failed;
		}

		return Super::Tick(Context, DeltaTime);
	}
	return EStateTreeRunStatus::Failed;
}

FSTT_Dash::FAITaskType* FSTT_Dash::PerformMoveTask(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	FAITaskType* AITaskPtr =
		InstanceData.AITaskPtr.Get() ? InstanceData.AITaskPtr.Get() : UAITask::NewAITask<FAITaskType>(*InstanceData.AIControllerPtr, *InstanceData.TaskOwner);
	if (AITaskPtr)
	{
		FVector Destination = FVector::ZeroVector;
		UNavigationSystemV1::K2_GetRandomReachablePointInRadius(InstanceData.AIControllerPtr, InstanceData.Destination, Destination, InstanceData.AcceptableRadius);
		AITaskPtr->SetUp(InstanceData.AIControllerPtr.Get(), Destination);
	}

	return AITaskPtr;
}
