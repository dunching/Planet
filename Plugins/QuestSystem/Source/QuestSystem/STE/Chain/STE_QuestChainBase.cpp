#include "STE_QuestChainBase.h"

#include "Kismet/GameplayStatics.h"

#include "QuestSubSystem.h"

void USTE_QuestChainBase::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	GloabVariable = NewObject<UGloabVariable_GuideThreadBase>();
}

void USTE_QuestChainBase::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);
}

void USTE_QuestChainBase::TreeStop(FStateTreeExecutionContext& Context)
{
	if (GuideThreadActorPtr)
	{
		auto GuideSubSystemPtr = UQuestSubSystem::GetInstance();
		GuideSubSystemPtr->OnGuideThreadStoped(GuideThreadActorPtr);
	}

	Super::TreeStop(Context);
}

void USTE_GuideThread_MainBase::TreeStart(
	FStateTreeExecutionContext& Context
)
{
	Super::TreeStart(Context);

	GloabVariable_MainPtr = NewObject<UGloabVariable_GuideThread_MainBase>();
}

void USTE_GuideThread_MainBase::TreeStop(FStateTreeExecutionContext& Context)
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
	}

	Super::TreeStop(Context);
}

void USTE_GuideThread_BrandBase::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	GloabVariable_Brand = NewObject<UGloabVariable_GuideThread_BrandBase>();
}

void USTE_GuideThread_BrandBase::TreeStop(FStateTreeExecutionContext& Context)
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
	}

	Super::TreeStop(Context);
}

void USTE_GuideThread_AreaBase::TreeStart(
	FStateTreeExecutionContext& Context
)
{
	Super::TreeStart(Context);

	GloabVariable_AreaBasePtr = NewObject<UGloabVariable_GuideThread_AreaBase>();
}

void USTE_GuideThread_AreaBase::TreeStop(FStateTreeExecutionContext& Context)
{
	Super::TreeStop(Context);
}
