#include "STT_ExcuteWorldProcessTask.h"

#include "Kismet/GameplayStatics.h"

#include "HumanCharacter_Player.h"
#include "TaskNode_Guide.h"
#include "GuideActor.h"
#include "GuideSystemGameplayTask.h"

FSTT_ExcuteWorldProcessTask::FSTT_ExcuteWorldProcessTask()
{
}

const UStruct* FSTT_ExcuteWorldProcessTask::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_ExcuteWorldProcessTask::EnterState(FStateTreeExecutionContext& Context,
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

void FSTT_ExcuteWorldProcessTask::ExitState(FStateTreeExecutionContext& Context,
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

EStateTreeRunStatus FSTT_ExcuteWorldProcessTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	Super::Tick(Context, DeltaTime);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() == EGameplayTaskState::Finished)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_ExcuteWorldProcessTask::PerformMoveTask(FStateTreeExecutionContext& Context) const
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
			case ETaskNodeType::kWorldProcess_MoveToPoint:
				{
					auto TaskNodeRef = Cast<UPAD_TaskNode_Guide_MoveToLocation>(InstanceData.TaskNodeRef.Get());
					if (!TaskNodeRef)
					{
						return EStateTreeRunStatus::Failed;
					}
					
					auto GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_MoveToLocation>(
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
			case ETaskNodeType::kWorldProcess_Conversation:
				{
					return EStateTreeRunStatus::Running;
				}
				break;
			}
		}
	}

	return EStateTreeRunStatus::Failed;
}
