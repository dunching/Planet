#include "STT_GuideInteraction.h"

#include "GuideInteractionBase.h"
#include "HumanRegularProcessor.h"
#include "InputProcessorSubSystem.h"
#include "STE_Interaction.h"
#include "HumanCharacter_Player.h"

EStateTreeRunStatus FSTT_GuideInteraction::EnterState(
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

EStateTreeRunStatus FSTT_GuideInteraction_BackToRegularProcessor::EnterState(
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

	UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();

	return EStateTreeRunStatus::Succeeded;
}
