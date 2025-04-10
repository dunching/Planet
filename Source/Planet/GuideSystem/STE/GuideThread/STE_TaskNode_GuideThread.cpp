#include "STE_TaskNode_GuideThread.h"

#include "Kismet/GameplayStatics.h"

#include "GuideActor.h"
#include "GuideSubSystem.h"
#include "GuideThread.h"
#include "HumanCharacter_Player.h"

void USTE_TaskNode_GuideThread::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);
}

void USTE_TaskNode_GuideThread::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);
}

void USTE_TaskNode_GuideThread::TreeStop(FStateTreeExecutionContext& Context)
{
	Super::TreeStop(Context);
}

void USTE_TaskNode_GuideMainThread::TreeStop(FStateTreeExecutionContext& Context)
{
	if (GuideActorPtr)
	{
		auto GuideSubSystemPtr = UGuideSubSystem::GetInstance();
		GuideSubSystemPtr->GuideThreadEnded(GuideActorPtr);
	}

	Super::TreeStop(Context);
}

void USTE_TaskNode_GuideBrandThread::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	GloabVariable = NewObject<UGloabVariable_GuideBrandThread>();
}

void USTE_TaskNode_GuideBrandThread::TreeStop(FStateTreeExecutionContext& Context)
{
	if (GloabVariable)
	{
		for (auto Iter : GloabVariable->TemporaryActorAry)
		{
			if (Iter)
			{
				Iter->Destroy();
			}
		}
		GloabVariable->TemporaryActorAry.Empty();
	}

	if (GuideActorPtr)
	{
		auto GuideSubSystemPtr = UGuideSubSystem::GetInstance();
		GuideSubSystemPtr->GuideThreadEnded(GuideActorPtr);
	}

	Super::TreeStop(Context);
}
