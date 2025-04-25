#include "STT_CharacterMonologue.h"

#include "Tasks/AITask.h"

#include "HumanAIController.h"
#include "AIComponent.h"
#include "AITask_CharacterMonologue.h"
#include "AITask_CharacterBase.h"
#include "ResourceBoxStateTreeComponent.h"
#include "HumanCharacter_AI.h"

EStateTreeRunStatus FSTT_CharacterMonologue::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	Super::EnterState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(
		InstanceData.AIControllerPtr->FindComponentByInterface(
			UGameplayTaskOwnerInterface::StaticClass()
		)
	);
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.AIControllerPtr;
	}

	// TODO -》spawn TaskNodeRef

	return PerformGameplayTask(
		Context
	);
}

void FSTT_CharacterMonologue::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() != EGameplayTaskState::Finished)
	{
		InstanceData.GameplayTaskPtr->ExternalCancel();
	}
	InstanceData.GameplayTaskPtr = nullptr;

	Super::ExitState(
		Context,
		Transition
	);
}

EStateTreeRunStatus FSTT_CharacterMonologue::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	// Super::Tick(Context, DeltaTime);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	if (!InstanceData.GameplayTaskPtr)
	{
		checkNoEntry();
		return EStateTreeRunStatus::Failed;
	}

	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() == EGameplayTaskState::Finished)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(
		Context,
		DeltaTime
	);
}

EStateTreeRunStatus FSTT_CharacterMonologue::PerformGameplayTask(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	InstanceData.GameplayTaskPtr = UAITask::NewAITask<UAITask_CharacterMonologue>(
		*InstanceData.AIControllerPtr, *InstanceData.TaskOwner
	);

	if (InstanceData.GameplayTaskPtr)
	{
		InstanceData.GameplayTaskPtr->SetUpController(
			InstanceData.CharacterPtr,
			InstanceData.AIControllerPtr
		);
		InstanceData.GameplayTaskPtr->SetUp(
			InstanceData.ConversationsAry
		);
		InstanceData.GameplayTaskPtr->ReadyForActivation();
	}

	return EStateTreeRunStatus::Running;
}
