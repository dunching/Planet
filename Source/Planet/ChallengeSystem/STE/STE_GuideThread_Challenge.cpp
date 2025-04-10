#include "STE_GuideThread_Challenge.h"

#include "Kismet/GameplayStatics.h"

#include "GuideActor.h"
#include "GuideSubSystem.h"
#include "GuideThread.h"
#include "GuideThreadChallengeActor.h"

void USTE_TaskNode_GuideChallengeThread::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	GloabVariable = NewObject<UGloabVariable_GuideChallengeThread>();
}

void USTE_TaskNode_GuideChallengeThread::TreeStop(FStateTreeExecutionContext& Context)
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
