
#include "STT_SwitchWalkState.h"

#include <NavigationSystem.h>

#include "AITask_DashToLeader.h"
#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "AITask_SwitchWalkState.h"

EStateTreeRunStatus FSTT_SwitchWalkState::EnterState(
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

void FSTT_SwitchWalkState::ExitState(
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

EStateTreeRunStatus FSTT_SwitchWalkState::Tick(
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

		if (InstanceData.bIscontinueCheck)
		{
			PerformMoveTask(Context);
			InstanceData.AITaskPtr->ConditionalPerformTask();
		}

		return Super::Tick(Context, DeltaTime);
	}
	return EStateTreeRunStatus::Failed;
}

FSTT_SwitchWalkState::FAITaskType* FSTT_SwitchWalkState::PerformMoveTask(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	FAITaskType* AITaskPtr =
		InstanceData.AITaskPtr.Get() ? InstanceData.AITaskPtr.Get() : UAITask::NewAITask<FAITaskType>(*InstanceData.AIControllerPtr, *InstanceData.TaskOwner);
	if (AITaskPtr)
	{
		AITaskPtr->SetUp(
			InstanceData.AIControllerPtr.Get(), 
			FVector::Distance(InstanceData.CharacterPtr->GetActorLocation(), InstanceData.TargetCharacterPtr->GetActorLocation()) > 500,
			InstanceData.bIscontinueCheck
		);
	}

	return AITaskPtr;
}
