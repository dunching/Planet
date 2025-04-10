#include "STT_GuideInteraction.h"

#include "Kismet/GameplayStatics.h"

#include "HumanCharacter_Player.h"

#include "GuideActor.h"
#include "GuideInteraction.h"
#include "GuideInteractionGameplayTask.h"
#include "GuideSubSystem.h"
#include "GuideSystemGameplayTask.h"
#include "HumanRegularProcessor.h"
#include "InputProcessorSubSystem.h"
#include "STE_TaskNode_Interaction.h"


EStateTreeRunStatus FSTT_GuideInteraction_Termination::EnterState(FStateTreeExecutionContext& Context,
                                                                  const FStateTreeTransitionResult& Transition) const
{
	return EStateTreeRunStatus::Succeeded;	
}

const UStruct* FSTT_GuideInteractionBase::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_GuideInteractionBase::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.GuideActorPtr)
	{
		if (InstanceData.GuideActorPtr->bWantToStop)
		{
			return EStateTreeRunStatus::Failed;
		}
	}
	else
	{
		return EStateTreeRunStatus::Failed;
	}

	return FStateTreeTaskBase::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_GuideInteractionBase::EnterState(FStateTreeExecutionContext& Context,
                                                          const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.PlayerCharacterPtr = Cast<AHumanCharacter_Player>(
		UGameplayStatics::GetPlayerCharacter(InstanceData.GuideActorPtr, 0));

	return Super::EnterState(Context, Transition);
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

	InstanceData.GloabVariable->TaskNodeResuleHelper.TaskID = InstanceData.TaskID;
	
	UGuideSubSystem::GetInstance()->GetCurrentGuideThread()->AddEvent(InstanceData.GloabVariable->TaskNodeResuleHelper);

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

	return PerformGameplayTask(Context);
}

EStateTreeRunStatus FSTT_GuideInteractionConversation::Tick(FStateTreeExecutionContext& Context,
                                                            const float DeltaTime) const
{
	// Super::Tick(Context, DeltaTime);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.GameplayTaskPtr)
	{
		checkNoEntry();
	}

	if (InstanceData.GuideActorPtr && InstanceData.GuideActorPtr->bWantToStop)
	{
		return EStateTreeRunStatus::Failed;
	}

	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() == EGameplayTaskState::Finished)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_GuideInteractionConversation::PerformGameplayTask(
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
			GameplayTaskPtr->SetGuideInteractionActor(InstanceData.GuideActorPtr);
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

	return PerformGameplayTask(Context);
}

EStateTreeRunStatus FSTT_GuideInteraction_BackToRegularProcessor::EnterState(FStateTreeExecutionContext& Context,
                                                                             const FStateTreeTransitionResult&
                                                                             Transition) const
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

	UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();

	return EStateTreeRunStatus::Succeeded;
}

EStateTreeRunStatus FSTT_GuideInteractionOption::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	// Super::Tick(Context, DeltaTime);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.GameplayTaskPtr)
	{
		checkNoEntry();
	}

	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() == EGameplayTaskState::Finished)
	{
		InstanceData.TaskNodeResuleHelper.Output_1 = InstanceData.GameplayTaskPtr->SelectedIndex;
		InstanceData.GloabVariable->TaskNodeResuleHelper.Output_1 = InstanceData.GameplayTaskPtr->SelectedIndex;

		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_GuideInteractionOption::PerformGameplayTask(FStateTreeExecutionContext& Context) const
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
