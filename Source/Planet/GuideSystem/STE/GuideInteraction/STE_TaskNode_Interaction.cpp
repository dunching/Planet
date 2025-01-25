#include "STE_TaskNode_Interaction.h"

#include "GuideActor.h"
#include "GuideInteractionActor.h"
#include "GuideSubSystem.h"

void USTE_TaskNode_Interaction::TreeStop(FStateTreeExecutionContext& Context)
{
	if (GuideActorPtr)
	{
		GuideActorPtr->OnGuideInteractionEnd.Broadcast();

		GuideActorPtr->Destroy();
	}
	
	Super::TreeStop(Context);
}
