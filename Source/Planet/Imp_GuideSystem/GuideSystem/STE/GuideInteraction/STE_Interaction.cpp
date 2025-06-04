#include "STE_Interaction.h"

#include "GuideActorBase.h"
#include "GuideInteraction.h"
#include "HumanCharacter_Player.h"
#include "HumanRegularProcessor.h"
#include "InputProcessorSubSystemBase.h"

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

void USTE_Interaction_ChallengeEntry::TreeStart(
	FStateTreeExecutionContext& Context
)
{
	Super::TreeStart(Context);
}

void USTE_Interaction_ChallengeEntry::TreeStop(
	FStateTreeExecutionContext& Context
)
{
	if (GuideActorPtr)
	{
		GuideActorPtr->Destroy();
	}
	
	Super::TreeStop(Context);
}
