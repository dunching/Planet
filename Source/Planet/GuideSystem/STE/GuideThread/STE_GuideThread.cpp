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
	if (GuideThreadActorPtr)
	{
		auto GuideSubSystemPtr = UGuideSubSystem::GetInstance();
		GuideSubSystemPtr->GuideThreadEnded(GuideThreadActorPtr);
	}

	Super::TreeStop(Context);
}

void USTE_GuideThread_Main::TreeStart(
	FStateTreeExecutionContext& Context
)
{
	Super::TreeStart(Context);

	GloabVariable_MainPtr = NewObject<UGloabVariable_GuideThread_Main>();
}

void USTE_GuideThread_Main::TreeStop(FStateTreeExecutionContext& Context)
{
	if (GloabVariable_MainPtr)
	{
		for (auto Iter : GloabVariable_MainPtr->TemporaryActorAry)
		{
			if (Iter)
			{
				Iter->Destroy();
			}
		}
		GloabVariable_MainPtr->TemporaryActorAry.Empty();
	}

	if (GuideThreadMainActorPtr)
	{
		auto GuideSubSystemPtr = UGuideSubSystem::GetInstance();
		GuideSubSystemPtr->GuideThreadEnded(GuideThreadMainActorPtr);
	}

	Super::TreeStop(Context);
}

void USTE_GuideThread_Brand::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	GloabVariable_Brand = NewObject<UGloabVariable_GuideThread_Brand>();
}

void USTE_GuideThread_Brand::TreeStop(FStateTreeExecutionContext& Context)
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

	if (GuideThreadBranchActorPtr)
	{
		auto GuideSubSystemPtr = UGuideSubSystem::GetInstance();
		GuideSubSystemPtr->GuideThreadEnded(GuideThreadBranchActorPtr);
	}

	Super::TreeStop(Context);
}

void USTE_GuideThread_Area::TreeStart(
	FStateTreeExecutionContext& Context
)
{
	Super::TreeStart(Context);

	GloabVariable_Area = NewObject<UGloabVariable_GuideThread_Area>();
}

void USTE_GuideThread_Area::TreeStop(FStateTreeExecutionContext& Context)
{
	if (GuideThreadAreaActorPtr)
	{
		auto GuideSubSystemPtr = UGuideSubSystem::GetInstance();
		GuideSubSystemPtr->GuideThreadEnded(GuideThreadAreaActorPtr);
	}

	Super::TreeStop(Context);
}
