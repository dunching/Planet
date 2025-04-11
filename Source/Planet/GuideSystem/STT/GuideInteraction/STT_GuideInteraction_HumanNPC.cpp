#include "STT_GuideInteraction_HumanNPC.h"

#include "Kismet/GameplayStatics.h"

#include "HumanCharacter_Player.h"

#include "GuideActor.h"
#include "GuideInteraction.h"
#include "GuideInteractionGameplayTask.h"
#include "GuideSubSystem.h"
#include "GuideSystemGameplayTask.h"
#include "HumanRegularProcessor.h"
#include "InputProcessorSubSystem.h"
#include "STE_Interaction.h"

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
		};

		BaseSetUp(GameplayTaskPtr);

		GameplayTaskPtr->SetUp(InstanceData.OptionAry, InstanceData.DurationTime);
		GameplayTaskPtr->ReadyForActivation();

		InstanceData.GameplayTaskPtr = GameplayTaskPtr;
	}

	return EStateTreeRunStatus::Running;
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
