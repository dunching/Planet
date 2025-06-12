#include "STT_QuestInteraction.h"

#include "QuestInteractionBase.h"
#include "HumanRegularProcessor.h"
#include "InputProcessorSubSystemBase.h"
#include "STE_Interaction.h"
#include "HumanCharacter_Player.h"
#include "InputProcessorSubSystem_Imp.h"

EStateTreeRunStatus FSTT_QuestInteraction::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
	) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	InstanceData.PlayerCharacterPtr = Cast<AHumanCharacter_Player>(
		UGameplayStatics::GetPlayerCharacter(InstanceData.GuideActorPtr, 0)
	);

	return Super::EnterState(Context, Transition);
}

EStateTreeRunStatus FSTT_QuestInteraction_BackToRegularProcessor::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult&
	Transition
) const
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

	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();

	return EStateTreeRunStatus::Succeeded;
}
