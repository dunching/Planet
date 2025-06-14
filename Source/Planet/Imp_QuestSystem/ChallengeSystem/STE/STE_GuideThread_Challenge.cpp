#include "STE_GuideThread_Challenge.h"

#include "Kismet/GameplayStatics.h"

#include "QuestsActorBase.h"
#include "QuestSubSystem.h"
#include "QuestChain.h"
#include "GuideThreadChallenge.h"

void USTE_TaskNode_GuideChallengeThread::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	GloabVariable_Challenge = NewObject<UGloabVariable_GuideThread_Challenge>();
}

void USTE_TaskNode_GuideChallengeThread::TreeStop(FStateTreeExecutionContext& Context)
{
	if (GloabVariable_Challenge)
	{
		for (auto Iter : GloabVariable_Challenge->TemporaryActorAry)
		{
			if (Iter)
			{
				Iter->Destroy();
			}
		}
		GloabVariable_Challenge->TemporaryActorAry.Empty();
	}

	if (GuideActorPtr)
	{
		auto GuideSubSystemPtr = UQuestSubSystem::GetInstance();
		GuideSubSystemPtr->OnGuideThreadStoped(GuideActorPtr);
	}

	Super::TreeStop(Context);
}
