#include "STE_Interaction.h"

#include "QuestsActorBase.h"
#include "QuestInteraction.h"
#include "HumanCharacter_Player.h"
#include "HumanRegularProcessor.h"
#include "InputProcessorSubSystemBase.h"

void USTE_Interaction_HumanCharacter_AI::TreeStart(
	FStateTreeExecutionContext& Context
	)
{
	Super::TreeStart(Context);
}

void USTE_Interaction_HumanCharacter_AI::TreeStop(
	FStateTreeExecutionContext& Context
	)
{
	FTSTicker::GetCoreTicker().AddTicker(
	                                     FTickerDelegate::CreateLambda(
	                                                                   [this](auto)
	                                                                   {
		                                                                   if (GuideActorPtr)
		                                                                   {
			                                                                   GuideActorPtr->Destroy();
		                                                                   }
		                                                                   return false;
	                                                                   }
	                                                                  )
	                                    );

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
