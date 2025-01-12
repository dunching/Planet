#include "STE_TaskNode.h"

#include "GuideActor.h"
#include "GuideSubSystem.h"

void USTE_TaskNode::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);
}

void USTE_TaskNode::TreeStop(FStateTreeExecutionContext& Context)
{
	if (GuideActorPtr)
	{
		auto GuideSubSystemPtr = UGuideSubSystem::GetInstance();
		GuideSubSystemPtr->OnGuideEnd.Broadcast(GuideActorPtr);
	}
	
	Super::TreeStop(Context);
}

void USTE_TaskNode::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);
}
