
#include "STT_ReleaseSkill.h"

#include <NavigationSystem.h>

#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "AITask_ReleaseSkill.h"

EStateTreeRunStatus FSTT_ReleaseSkill::EnterState(
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

	return PerformMoveTask(Context);
}

void FSTT_ReleaseSkill::ExitState(
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

EStateTreeRunStatus FSTT_ReleaseSkill::Tick(
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

EStateTreeRunStatus FSTT_ReleaseSkill::PerformMoveTask(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.AITaskPtr)
	{
		InstanceData.AITaskPtr =
			UAITask::NewAITask<FAITaskType>(*InstanceData.AIControllerPtr, *InstanceData.TaskOwner);
	}

	if (InstanceData.AITaskPtr)
	{
		InstanceData.AITaskPtr->SetUp(InstanceData.CharacterPtr.Get());

		if (InstanceData.AITaskPtr->IsActive())
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
	return EStateTreeRunStatus::Failed;
}
