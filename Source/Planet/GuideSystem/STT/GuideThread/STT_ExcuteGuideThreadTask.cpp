#include "STT_ExcuteGuideThreadTask.h"

#include "Kismet/GameplayStatics.h"

#include "HumanCharacter_Player.h"
#include "TaskNode_Guide.h"
#include "GuideActor.h"
#include "GuideSystemGameplayTask.h"
#include "GuideThreadActor.h"
#include "GuideThreadGameplayTask.h"
#include "HumanCharacter_AI.h"
#include "TaskNode_Interaction.h"

EStateTreeRunStatus FSTT_ExcuteGuideThreadFaileTask::EnterState(FStateTreeExecutionContext& Context,
                                                                const FStateTreeTransitionResult& Transition) const
{
	// TODO 任务失败时 

	checkNoEntry();
	
	return Super::EnterState(Context, Transition);
}

const UStruct* FSTT_ExcuteGuideThreadMonologueTask::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_ExcuteGuideThreadMonologueTask::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	Super::EnterState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(
		InstanceData.GuideActorPtr->FindComponentByInterface(UGameplayTaskOwnerInterface::StaticClass())
	);
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.GuideActorPtr;
	}

	InstanceData.PlayerCharacterPtr = Cast<AHumanCharacter_Player>(
		UGameplayStatics::GetPlayerCharacter(InstanceData.GuideActorPtr, 0));

	// TODO -》spawn TaskNodeRef

	return PerformMoveTask(Context);
}

EStateTreeRunStatus FSTT_ExcuteGuideThreadMonologueTask::Tick(FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	// Super::Tick(Context, DeltaTime);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if(!InstanceData.GameplayTaskPtr)
	{
		checkNoEntry();
	}
	
	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() == EGameplayTaskState::Finished)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_ExcuteGuideThreadMonologueTask::PerformMoveTask(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	auto GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_Guide_Monologue>(
		*InstanceData.TaskOwner);

	if (GameplayTaskPtr)
	{
		GameplayTaskPtr->SetPlayerCharacter(InstanceData.PlayerCharacterPtr);
		GameplayTaskPtr->SetUp(InstanceData.ConversationsAry);
		GameplayTaskPtr->ReadyForActivation();

		InstanceData.GameplayTaskPtr = GameplayTaskPtr;
	}

	return EStateTreeRunStatus::Running;
}

FTaskNodeDescript FSTT_ExcuteGuideThreadMonologueTask::GetTaskNodeDescripton(FStateTreeExecutionContext& Context) const
{
	FTaskNodeDescript TaskNodeDescript;

	TaskNodeDescript.bIsFreshPreviouDescription = false;
	
	return TaskNodeDescript;
}

const UStruct* FSTT_ExcuteGuideThreadBaseTask::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_ExcuteGuideThreadBaseTask::EnterState(FStateTreeExecutionContext& Context,
                                                               const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.GuideActorPtr)
	{
		InstanceData.GuideActorPtr->UpdateCurrentTaskNode(GetTaskNodeDescripton(Context));
	}
	else
	{
		return EStateTreeRunStatus::Failed;
	}

	return FStateTreeTaskBase::EnterState(Context, Transition);
}

FTaskNodeDescript FSTT_ExcuteGuideThreadBaseTask::GetTaskNodeDescripton(FStateTreeExecutionContext& Context) const
{
	FTaskNodeDescript TaskNodeDescript;

	return TaskNodeDescript;
}

const UStruct* FSTT_ExcuteGuideThreadGenraricTask::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_ExcuteGuideThreadGenraricTask::EnterState(FStateTreeExecutionContext& Context,
                                                            const FStateTreeTransitionResult& Transition) const
{
	Super::EnterState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
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

void FSTT_ExcuteGuideThreadGenraricTask::ExitState(FStateTreeExecutionContext& Context,
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

EStateTreeRunStatus FSTT_ExcuteGuideThreadGenraricTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	// Super::Tick(Context, DeltaTime);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if(!InstanceData.GameplayTaskPtr)
	{
		checkNoEntry();
	}
	
	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() == EGameplayTaskState::Finished)
	{
		switch (InstanceData.TaskNodeRef->TaskNodeType)
		{
		case ETaskNodeType::kGuide_ConversationWithTarget:
			{
				InstanceData.LastTaskOut = Cast<UGameplayTask_Guide_ConversationWithTarget>(InstanceData.GameplayTaskPtr)->SelectedIndex;
			}
			break;
		}
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_ExcuteGuideThreadGenraricTask::PerformMoveTask(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.TaskNodeRef.IsNull())
	{
	}
	else
	{
		if (InstanceData.TaskNodeRef->TaskNodeState == ETaskNodeState::kWaitExcute)
		{
			auto BaseSetUp = [InstanceData](auto GameplayTaskPtr)
			{
				GameplayTaskPtr->SetPlayerCharacter(InstanceData.PlayerCharacterPtr);
				GameplayTaskPtr->SetTaskID(InstanceData.TaskID);
				GameplayTaskPtr->SetGuideActor(InstanceData.GuideActorPtr);
			};
			
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
			case ETaskNodeType::kGuide_AddToTarget:
				{
					auto TaskNodeRef = Cast<UPAD_TaskNode_Guide_AddToTarget>(InstanceData.TaskNodeRef.Get());
					if (!TaskNodeRef)
					{
						return EStateTreeRunStatus::Failed;
					}

					auto GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_Guide_AddToTarget>(
						*InstanceData.TaskOwner);

					if (GameplayTaskPtr)
					{
						GameplayTaskPtr->SetPlayerCharacter(InstanceData.PlayerCharacterPtr);
						GameplayTaskPtr->SetUp(TaskNodeRef);
						GameplayTaskPtr->ReadyForActivation();

						InstanceData.GameplayTaskPtr = GameplayTaskPtr;
					}

					return EStateTreeRunStatus::Running;
				}
				break;
			case ETaskNodeType::kGuide_ConversationWithTarget:
				{
					auto TaskNodeRef = Cast<UPAD_TaskNode_Guide_ConversationWithTarget>(InstanceData.TaskNodeRef.Get());
					if (!TaskNodeRef)
					{
						return EStateTreeRunStatus::Failed;
					}

					auto GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_Guide_ConversationWithTarget>(
						*InstanceData.TaskOwner);

					if (GameplayTaskPtr)
					{
						BaseSetUp(GameplayTaskPtr);
						
						GameplayTaskPtr->SetUp(TaskNodeRef);
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

FTaskNodeDescript FSTT_ExcuteGuideThreadGenraricTask::GetTaskNodeDescripton(FStateTreeExecutionContext& Context) const
{
	FTaskNodeDescript TaskNodeDescript;

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.GuideActorPtr)
	{
			if (!InstanceData.TaskNodeRef.IsNull())
			{
				InstanceData.TaskNodeRef.LoadSynchronous();
			}

			switch (InstanceData.TaskNodeRef->TaskNodeType)
			{
			case ETaskNodeType::kGuide_MoveToPoint:
				{
					TaskNodeDescript.Name = InstanceData.Name;
					if (InstanceData.Description.IsEmpty())
					{
						TaskNodeDescript.Description = TEXT("移动至目标点");
					}
					else
					{
						TaskNodeDescript.Description = InstanceData.Description;
					}
				}
				break;
			case ETaskNodeType::kGuide_PressKey:
				{
					auto TaskNodeRef = Cast<UPAD_TaskNode_Guide_PressKey>(InstanceData.TaskNodeRef.Get());
					if (TaskNodeRef)
					{
						TaskNodeDescript.Name = InstanceData.Name;
						TaskNodeDescript.Description = FString::Printf(TEXT("Press %s key"), *TaskNodeRef->Key.ToString());
					}
				}
				break;
			case ETaskNodeType::kGuide_Monologue:
				{
					TaskNodeDescript.Name = InstanceData.Name;
					TaskNodeDescript.Description = InstanceData.Description;
				}
				break;
			case ETaskNodeType::kGuide_AddToTarget:
				{
					TaskNodeDescript.Name = InstanceData.Name;
					TaskNodeDescript.Description = InstanceData.Description;
					
					TaskNodeDescript.bIsFreshPreviouDescription = false;
				}
				break;
			case ETaskNodeType::kGuide_ConversationWithTarget:
				{
					auto TaskNodeRef = Cast<UPAD_TaskNode_Guide_ConversationWithTarget>(InstanceData.TaskNodeRef.Get());
					if (TaskNodeRef)
					{
						TaskNodeDescript.Name = InstanceData.Name;
						TaskNodeDescript.Description = FString::Printf(TEXT("前往与【%s】对话"), *TaskNodeRef->TargetCharacterPtr->GetName());
					}
				}
				break;
			};
	}

	return TaskNodeDescript;
}
