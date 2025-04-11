#include "STE_GuideThread.h"

#include "Kismet/GameplayStatics.h"

#include "GuideActor.h"
#include "GuideSubSystem.h"
#include "GuideThread.h"
#include "HumanCharacter_Player.h"

void USTE_GuideThread::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	GloabVariable = NewObject<UGloabVariable_GuideThread>();
}

void USTE_GuideThread::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);
}

void USTE_GuideThread::TreeStop(FStateTreeExecutionContext& Context)
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

	GloabVariable_Brand = NewObject<UGloabVariable_GuideThread_Brand>();
}

void USTE_TaskNode_GuideBrandThread::TreeStop(FStateTreeExecutionContext& Context)
{
	if (GloabVariable_Brand)
	{
		for (auto Iter : GloabVariable_Brand->TemporaryActorAry)
		{
			if (Iter)
			{
				Iter->Destroy();
			}
		}
		GloabVariable_Brand->TemporaryActorAry.Empty();
	}

	if (GuideActorPtr)
	{
		auto GuideSubSystemPtr = UGuideSubSystem::GetInstance();
		GuideSubSystemPtr->GuideThreadEnded(GuideActorPtr);
	}

	Super::TreeStop(Context);
}

void USTE_TaskNode_GuideThread_Area::TreeStart(
	FStateTreeExecutionContext& Context
)
{
	Super::TreeStart(Context);

	GloabVariable_Area = NewObject<UGloabVariable_GuideThread_Area>();
}

void USTE_TaskNode_GuideThread_Area::TreeStop(FStateTreeExecutionContext& Context)
{
	if (GuideActorPtr)
	{
		auto GuideSubSystemPtr = UGuideSubSystem::GetInstance();
		GuideSubSystemPtr->GuideThreadEnded(GuideActorPtr);
	}

	Super::TreeStop(Context);
}
