#include "STE_Interaction.h"

#include "GuideActor.h"
#include "GuideInteraction.h"
#include "HumanCharacter_Player.h"
#include "HumanRegularProcessor.h"
#include "InputProcessorSubSystem.h"

void USTE_Interaction::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	GloabVariable = NewObject<UGloabVariable_Interaction>();
}

void USTE_Interaction::TreeStop(FStateTreeExecutionContext& Context)
{
	UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
	
	Super::TreeStop(Context);
}

void USTE_Interaction_HumanCharacter_AI::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);
}

void USTE_Interaction_HumanCharacter_AI::TreeStop(FStateTreeExecutionContext& Context)
{
	if (GuideActorPtr)
	{
		GuideActorPtr->Destroy();
	}
	
	Super::TreeStop(Context);
}
