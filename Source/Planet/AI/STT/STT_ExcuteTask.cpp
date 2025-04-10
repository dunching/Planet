#include "STT_ExcuteTask.h"

#include "Tasks/AITask.h"

#include "HumanAIController.h"
#include "AIComponent.h"
#include "AITask_ExcuteTask_Base.h"
#include "AITask_Conversation.h"
#include "ResourceBoxStateTreeComponent.h"
#include "HumanCharacter_AI.h"

EStateTreeRunStatus FSTT_ExcuteTask_Automatic::EnterState(FStateTreeExecutionContext& Context,
                                                const FStateTreeTransitionResult& Transition) const
{
	FStateTreeTaskBase::EnterState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.CharacterPtr)
	{
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(
		InstanceData.AIControllerPtr->FindComponentByInterface(UGameplayTaskOwnerInterface::StaticClass())
	);
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.AIControllerPtr;
	}

	return PerformGameplayTask(Context);
}

void FSTT_ExcuteTask_Automatic::ExitState(FStateTreeExecutionContext& Context,
                                const FStateTreeTransitionResult& Transition) const
{
	FStateTreeTaskBase::ExitState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
}

EStateTreeRunStatus FSTT_ExcuteTask_Automatic::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_ExcuteTask_Automatic::PerformGameplayTask(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	return EStateTreeRunStatus::Failed;
}
