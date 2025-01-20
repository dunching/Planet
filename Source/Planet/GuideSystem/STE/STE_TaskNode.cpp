#include "STE_TaskNode.h"

#include "GuideActor.h"
#include "GuideSubSystem.h"

void USTE_TaskNode_GuideMainThread::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);
}

void USTE_TaskNode_GuideMainThread::TreeStop(FStateTreeExecutionContext& Context)
{
	if (GuideActorPtr)
	{
		auto GuideSubSystemPtr = UGuideSubSystem::GetInstance();
		GuideSubSystemPtr->OnGuideEnd.Broadcast(GuideActorPtr);
	}
	
	Super::TreeStop(Context);
}

void USTE_TaskNode_GuideMainThread::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);
}

void USTE_TaskNode_Interaction::TreeStop(FStateTreeExecutionContext& Context)
{
	if (GuideActorPtr)
	{
		GuideActorPtr->OnGuideInteractionEnd.Broadcast();

		GuideActorPtr->Destroy();
	}
	
	Super::TreeStop(Context);
}
