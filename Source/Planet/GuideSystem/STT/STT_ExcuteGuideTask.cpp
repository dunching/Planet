#include "STT_ExcuteGuideTask.h"

#include "Kismet/GameplayStatics.h"

#include "HumanCharacter_Player.h"
#include "TaskNode_Guide.h"
#include "GuideActor.h"
#include "GuideSystemGameplayTask.h"
#include "TaskNode_Interaction.h"

FSTT_ExcuteGuideTask::FSTT_ExcuteGuideTask()
{
}

const UStruct* FSTT_ExcuteGuideTask::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_ExcuteGuideTask::EnterState(FStateTreeExecutionContext& Context,
                                                            const FStateTreeTransitionResult& Transition) const
{
	Super::EnterState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.GuideActorPtr)
	{
		InstanceData.GuideActorPtr->UpdateCurrentTaskNode(InstanceData.TaskNodeRef);
	}
	else
	{
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(
		InstanceData.GuideActorPtr->FindComponentByInterface(UGameplayTaskOwnerInterface::StaticClass())
	);
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.GuideActorPtr;
	}

	if (!InstanceData.TaskNodeRef.IsNull())
	{
		InstanceData.TaskNodeRef.LoadSynchronous();
	}

	InstanceData.PlayerCharacterPtr = Cast<AHumanCharacter_Player>(
		UGameplayStatics::GetPlayerCharacter(InstanceData.GuideActorPtr, 0));

	// TODO -》spawn TaskNodeRef

	return PerformMoveTask(Context);
}

void FSTT_ExcuteGuideTask::ExitState(FStateTreeExecutionContext& Context,
                                            const FStateTreeTransitionResult& Transition) const
{
	Super::ExitState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() != EGameplayTaskState::Finished)
	{
		InstanceData.GameplayTaskPtr->ExternalCancel();
	}
	InstanceData.GameplayTaskPtr = nullptr;
}

EStateTreeRunStatus FSTT_ExcuteGuideTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	Super::Tick(Context, DeltaTime);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() == EGameplayTaskState::Finished)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_ExcuteGuideTask::PerformMoveTask(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.TaskNodeRef.IsNull())
	{
	}
	else
	{
		if (InstanceData.TaskNodeRef->TaskNodeState == ETaskNodeState::kWaitExcute)
		{
			switch (InstanceData.TaskNodeRef->TaskNodeType)
			{
			case ETaskNodeType::kGuide_MoveToPoint:
				{
					auto TaskNodeRef = Cast<UPAD_TaskNode_Guide_MoveToLocation>(InstanceData.TaskNodeRef.Get());
					if (!TaskNodeRef)
					{
						return EStateTreeRunStatus::Failed;
					}

					auto GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_Guide_MoveToLocation>(
						*InstanceData.TaskOwner);

					if (GameplayTaskPtr)
					{
						GameplayTaskPtr->SetPlayerCharacter(InstanceData.PlayerCharacterPtr);
						GameplayTaskPtr->SetUp(TaskNodeRef->TargetLocation, TaskNodeRef->ReachedRadius);
						GameplayTaskPtr->ReadyForActivation();

						InstanceData.GameplayTaskPtr = GameplayTaskPtr;
					}

					return EStateTreeRunStatus::Running;
				}
				break;
			case ETaskNodeType::kGuide_PressKey:
				{
					auto TaskNodeRef = Cast<UPAD_TaskNode_Guide_PressKey>(InstanceData.TaskNodeRef.Get());
					if (!TaskNodeRef)
					{
						return EStateTreeRunStatus::Failed;
					}

					auto GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_Guide_WaitInputKey>(
						*InstanceData.TaskOwner);

					if (GameplayTaskPtr)
					{
						GameplayTaskPtr->SetPlayerCharacter(InstanceData.PlayerCharacterPtr);
						GameplayTaskPtr->Key = TaskNodeRef->Key;
						GameplayTaskPtr->ReadyForActivation();

						InstanceData.GameplayTaskPtr = GameplayTaskPtr;
					}

					return EStateTreeRunStatus::Running;
				}
				break;
			case ETaskNodeType::kGuide_Monologue:
				{
					auto TaskNodeRef = Cast<UPAD_TaskNode_Guide_Monologue>(InstanceData.TaskNodeRef.Get());
					if (!TaskNodeRef)
					{
						return EStateTreeRunStatus::Failed;
					}

					auto GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_Guide_Monologue>(
						*InstanceData.TaskOwner);

					if (GameplayTaskPtr)
					{
						GameplayTaskPtr->SetPlayerCharacter(InstanceData.PlayerCharacterPtr);
						GameplayTaskPtr->SetUp(TaskNodeRef->ConversationsAry);
						GameplayTaskPtr->ReadyForActivation();

						InstanceData.GameplayTaskPtr = GameplayTaskPtr;
					}

					return EStateTreeRunStatus::Running;
				}
				break;
			};
		}
	}

	return EStateTreeRunStatus::Failed;
}

FSTT_ExcuteGuideInteractionTask::FSTT_ExcuteGuideInteractionTask()
{
}

const UStruct* FSTT_ExcuteGuideInteractionTask::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_ExcuteGuideInteractionTask::EnterState(FStateTreeExecutionContext& Context,
                                                            const FStateTreeTransitionResult& Transition) const
{
	Super::EnterState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.GuideActorPtr)
	{
	}
	else
	{
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(
		InstanceData.GuideActorPtr->FindComponentByInterface(UGameplayTaskOwnerInterface::StaticClass())
	);
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.GuideActorPtr;
	}

	if (!InstanceData.TaskNodeRef.IsNull())
	{
		InstanceData.TaskNodeRef.LoadSynchronous();
	}

	InstanceData.PlayerCharacterPtr = Cast<AHumanCharacter_Player>(
		UGameplayStatics::GetPlayerCharacter(InstanceData.GuideActorPtr, 0));

	// TODO -》spawn TaskNodeRef

	return PerformMoveTask(Context);
}

void FSTT_ExcuteGuideInteractionTask::ExitState(FStateTreeExecutionContext& Context,
                                            const FStateTreeTransitionResult& Transition) const
{
	Super::ExitState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() != EGameplayTaskState::Finished)
	{
		InstanceData.GameplayTaskPtr->ExternalCancel();
	}
	InstanceData.GameplayTaskPtr = nullptr;
}

EStateTreeRunStatus FSTT_ExcuteGuideInteractionTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	Super::Tick(Context, DeltaTime);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() == EGameplayTaskState::Finished)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_ExcuteGuideInteractionTask::PerformMoveTask(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.TaskNodeRef.IsNull())
	{
	}
	else
	{
		if (InstanceData.TaskNodeRef->TaskNodeState == ETaskNodeState::kWaitExcute)
		{
			switch (InstanceData.TaskNodeRef->TaskNodeType)
			{
			case ETaskNodeType::kInteraction_Conversation:
				{
					auto TaskNodeRef = Cast<UPAD_TaskNode_Interaction_Conversation>(InstanceData.TaskNodeRef.LoadSynchronous());
					if (!TaskNodeRef)
					{
						return EStateTreeRunStatus::Failed;
					}

					auto GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_Interaction_Conversation>(
						*InstanceData.TaskOwner);

					if (GameplayTaskPtr)
					{
						GameplayTaskPtr->SetPlayerCharacter(InstanceData.PlayerCharacterPtr);
						GameplayTaskPtr->SetUp(TaskNodeRef->ConversationsAry, InstanceData.TargetCharacterPtr);
						GameplayTaskPtr->ReadyForActivation();

						InstanceData.GameplayTaskPtr = GameplayTaskPtr;
					}

					return EStateTreeRunStatus::Running;
				}
				break;
			case ETaskNodeType::kInteraction_Option:
				{
					auto TaskNodeRef = Cast<UPAD_TaskNode_Interaction_Option>(InstanceData.TaskNodeRef.LoadSynchronous());
					if (!TaskNodeRef)
					{
						return EStateTreeRunStatus::Failed;
					}

					auto GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_Interaction_Option>(
						*InstanceData.TaskOwner);

					if (GameplayTaskPtr)
					{
						GameplayTaskPtr->SetPlayerCharacter(InstanceData.PlayerCharacterPtr);
						GameplayTaskPtr->SetUp(InstanceData.TaskNodeRef.LoadSynchronous(), InstanceData.TargetCharacterPtr);
						GameplayTaskPtr->ReadyForActivation();

						InstanceData.GameplayTaskPtr = GameplayTaskPtr;
					}

					return EStateTreeRunStatus::Running;
				}
				break;
			}
		}
	}

	return EStateTreeRunStatus::Failed;
}
