#include "STT_ExcuteTask.h"

#include "Tasks/AITask.h"

#include "HumanAIController.h"
#include "AIComponent.h"
#include "AITask_ExcuteTask_Base.h"
#include "AITask_Conversation.h"
#include "ResourceBoxStateTreeComponent.h"
#include "HumanCharacter_AI.h"
#include "TaskNode.h"

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

	return PerformMoveTask(Context);
}

void FSTT_ExcuteTask_Automatic::ExitState(FStateTreeExecutionContext& Context,
                                const FStateTreeTransitionResult& Transition) const
{
	FStateTreeTaskBase::ExitState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.AITaskPtr && InstanceData.AITaskPtr->GetState() != EGameplayTaskState::Finished)
	{
		InstanceData.AITaskPtr->ExternalCancel();
	}
	InstanceData.AITaskPtr = nullptr;
}

EStateTreeRunStatus FSTT_ExcuteTask_Automatic::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.AITaskPtr && InstanceData.AITaskPtr->GetState() == EGameplayTaskState::Finished)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_ExcuteTask_Automatic::PerformMoveTask(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.CharacterPtr->GetAIComponent()->PresetTaskNodesAry.IsEmpty())
	{
		InstanceData.TaskNodesAry = InstanceData.CharacterPtr->GetAIComponent()->PresetTaskNodesAry;

		if (InstanceData.CurrentTaskNodePtr)
		{
		}
		else
		{
			for (auto Iter : InstanceData.TaskNodesAry)
			{
				Iter.LoadSynchronous();
				if (Iter->TaskNodeState == ETaskNodeState::kWaitExcute)
				{
					switch (Iter->TaskNodeType)
					{
					case ETaskNodeType::kAutomatic_Conversatin:
					case ETaskNodeType::kConversatin:
						{
							if (!InstanceData.AITaskPtr)
							{
								InstanceData.AITaskPtr =
									UAITask::NewAITask<FAITaskType_Conversation>(
										*InstanceData.AIControllerPtr, *InstanceData.TaskOwner);
							}

							if (InstanceData.AITaskPtr)
							{
								InstanceData.AITaskPtr->SetUp(Iter, InstanceData.CharacterPtr);

								if (InstanceData.AITaskPtr->IsActive())
								{
									InstanceData.AITaskPtr->ConditionalPerformTask();
								}
								else
								{
									InstanceData.AITaskPtr->ReadyForActivation();
								}
							}
							return EStateTreeRunStatus::Running;
						}
						break;
					case ETaskNodeType::kTest:
						break;
					case ETaskNodeType::kNone:
						break;
					default: ;
					}
				}
			}
		}
	}
	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus FSTT_ExcuteTask_Temporary::EnterState(FStateTreeExecutionContext& Context,
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

	return PerformMoveTask(Context);
}

void FSTT_ExcuteTask_Temporary::ExitState(FStateTreeExecutionContext& Context,
                                const FStateTreeTransitionResult& Transition) const
{
	FStateTreeTaskBase::ExitState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.AITaskPtr && InstanceData.AITaskPtr->GetState() != EGameplayTaskState::Finished)
	{
		InstanceData.AITaskPtr->ExternalCancel();
	}
	InstanceData.AITaskPtr = nullptr;
}

EStateTreeRunStatus FSTT_ExcuteTask_Temporary::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.AITaskPtr && InstanceData.AITaskPtr->GetState() == EGameplayTaskState::Finished)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_ExcuteTask_Temporary::PerformMoveTask(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.CharacterPtr->GetAIComponent()->TemporaryTaskNodesAry.IsEmpty())
	{
		InstanceData.TaskNodesAry = InstanceData.CharacterPtr->GetAIComponent()->TemporaryTaskNodesAry;

		if (InstanceData.CurrentTaskNodePtr)
		{
		}
		else
		{
			for (auto Iter : InstanceData.TaskNodesAry)
			{
				switch (Iter->TaskNodeType)
				{
				case ETaskNodeType::kTemporary_Conversation:
					{
						if (!InstanceData.AITaskPtr)
						{
							InstanceData.AITaskPtr =
								UAITask::NewAITask<UAITask_Conversation_SingleSentence>(
									*InstanceData.AIControllerPtr, *InstanceData.TaskOwner);
						}

						if (InstanceData.AITaskPtr)
						{
							InstanceData.AITaskPtr->SetUp(Iter, InstanceData.CharacterPtr);

							if (InstanceData.AITaskPtr->IsActive())
							{
								InstanceData.AITaskPtr->ConditionalPerformTask();
							}
							else
							{
								InstanceData.AITaskPtr->ReadyForActivation();
							}
						}

						InstanceData.CharacterPtr->GetAIComponent()->TemporaryTaskNodesAry.Pop();
						
						return EStateTreeRunStatus::Running;
					}
					break;
				default: ;
				}
			}
		}
	}
	return EStateTreeRunStatus::Failed;
}
