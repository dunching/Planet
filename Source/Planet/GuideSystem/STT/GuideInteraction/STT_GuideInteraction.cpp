#include "STT_GuideInteraction.h"

#include "Kismet/GameplayStatics.h"

#include "HumanCharacter_Player.h"
#include "TaskNode_Guide.h"
#include "GuideActor.h"
#include "GuideInteractionActor.h"
#include "GuideInteractionGameplayTask.h"
#include "GuideSystemGameplayTask.h"
#include "TaskNode_Interaction.h"

EStateTreeRunStatus FSTT_GuideInteractionFaile::EnterState(FStateTreeExecutionContext& Context,
                                                                     const FStateTreeTransitionResult& Transition) const
{
	// TODO 任务失败时 

	checkNoEntry();
	
	return FStateTreeTaskBase::EnterState(Context, Transition);
}

const UStruct* FSTT_GuideInteractionBase::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_GuideInteractionBase::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.PlayerCharacterPtr = Cast<AHumanCharacter_Player>(
		UGameplayStatics::GetPlayerCharacter(InstanceData.GuideActorPtr, 0));
	
	return Super::EnterState(Context, Transition);
}

FSTT_GuideInteractionGeneric::FSTT_GuideInteractionGeneric()
{
}

const UStruct* FSTT_GuideInteractionGeneric::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_GuideInteractionGeneric::EnterState(FStateTreeExecutionContext& Context,
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

void FSTT_GuideInteractionGeneric::ExitState(FStateTreeExecutionContext& Context,
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

EStateTreeRunStatus FSTT_GuideInteractionGeneric::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
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

EStateTreeRunStatus FSTT_GuideInteractionGeneric::PerformMoveTask(FStateTreeExecutionContext& Context) const
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
				GameplayTaskPtr->SetTargetCharacterPtr(InstanceData.TargetCharacterPtr);
			};
			
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
						BaseSetUp(GameplayTaskPtr);
						
						GameplayTaskPtr->SetUp(TaskNodeRef->ConversationsAry);
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
						BaseSetUp(GameplayTaskPtr);
						
						GameplayTaskPtr->SetUp(InstanceData.TaskNodeRef.LoadSynchronous());
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

const UStruct* FSTT_GuideInteractionNotify::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_GuideInteractionNotify::EnterState(FStateTreeExecutionContext& Context,
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

	// TODO -》spawn TaskNodeRef

	return PerformMoveTask(Context);
}

EStateTreeRunStatus FSTT_GuideInteractionNotify::PerformMoveTask(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.GameplayTaskPtr= UGameplayTask::NewTask<UGameplayTask_Interaction_NotifyGuideThread>(
		*InstanceData.TaskOwner);

	if (InstanceData.GameplayTaskPtr)
	{
		auto BaseSetUp = [InstanceData](auto GameplayTaskPtr)
		{
			GameplayTaskPtr->SetPlayerCharacter(InstanceData.PlayerCharacterPtr);
			GameplayTaskPtr->SetTargetCharacterPtr(InstanceData.TargetCharacterPtr);
		};
			
		BaseSetUp(InstanceData.GameplayTaskPtr);
						
		InstanceData.GameplayTaskPtr->SetUp(InstanceData.GuideInteractionActorClass, InstanceData.NotifyTaskID, InstanceData.Index);
		InstanceData.GameplayTaskPtr->ReadyForActivation();
	}

	return EStateTreeRunStatus::Succeeded;
}

const UStruct* FSTT_GuideInteractionConversation::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_GuideInteractionConversation::EnterState(FStateTreeExecutionContext& Context,
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

	InstanceData.PlayerCharacterPtr = Cast<AHumanCharacter_Player>(
		UGameplayStatics::GetPlayerCharacter(InstanceData.GuideActorPtr, 0));

	// TODO -》spawn TaskNodeRef

	return PerformMoveTask(Context);
}

EStateTreeRunStatus FSTT_GuideInteractionConversation::Tick(FStateTreeExecutionContext& Context,
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

EStateTreeRunStatus FSTT_GuideInteractionConversation::PerformMoveTask(
	FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	auto GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_Interaction_Conversation>(
		*InstanceData.TaskOwner);

	if (GameplayTaskPtr)
	{
		auto BaseSetUp = [InstanceData](auto GameplayTaskPtr)
		{
			GameplayTaskPtr->SetPlayerCharacter(InstanceData.PlayerCharacterPtr);
			GameplayTaskPtr->SetTargetCharacterPtr(InstanceData.TargetCharacterPtr);
		};
					
		BaseSetUp(GameplayTaskPtr);
							
		GameplayTaskPtr->SetUp(InstanceData.ConversationsAry);
		GameplayTaskPtr->ReadyForActivation();

		InstanceData.GameplayTaskPtr = GameplayTaskPtr;
	}

	return EStateTreeRunStatus::Running;
}

 const UStruct* FSTT_GuideInteractionOption::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_GuideInteractionOption::EnterState(FStateTreeExecutionContext& Context,
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

	InstanceData.PlayerCharacterPtr = Cast<AHumanCharacter_Player>(
		UGameplayStatics::GetPlayerCharacter(InstanceData.GuideActorPtr, 0));

	// TODO -》spawn TaskNodeRef

	return PerformMoveTask(Context);
}

EStateTreeRunStatus FSTT_GuideInteractionOption::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	// Super::Tick(Context, DeltaTime);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if(!InstanceData.GameplayTaskPtr)
	{
		checkNoEntry();
	}
	
	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() == EGameplayTaskState::Finished)
	{
		InstanceData.TaskNodeResuleHelper.Output_1 = InstanceData.GameplayTaskPtr->SelectedIndex;
		
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_GuideInteractionOption::PerformMoveTask(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	auto GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_Interaction_Option>(
		*InstanceData.TaskOwner);

	if (GameplayTaskPtr)
	{
		auto BaseSetUp = [InstanceData](auto GameplayTaskPtr)
		{
			GameplayTaskPtr->SetPlayerCharacter(InstanceData.PlayerCharacterPtr);
			GameplayTaskPtr->SetTargetCharacterPtr(InstanceData.TargetCharacterPtr);
		};
			
		BaseSetUp(GameplayTaskPtr);
		
		GameplayTaskPtr->SetUp(InstanceData.OptionAry, InstanceData.DurationTime);
		GameplayTaskPtr->ReadyForActivation();

		InstanceData.GameplayTaskPtr = GameplayTaskPtr;
	}

	return EStateTreeRunStatus::Running;
}
