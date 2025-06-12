#include "STT_QuestInteractionBase.h"

#include "Kismet/GameplayStatics.h"

#include "QuestInteractionBase.h"
#include "QuestSubSystem.h"
#include "QuestChainBase.h"
#include "STE_QuestInteractionBase.h"

EStateTreeRunStatus FSTT_GuideInteraction_Termination::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	return EStateTreeRunStatus::Succeeded;
}

const UStruct* FSTT_QuestInteractionBase::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_QuestInteractionBase::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.GuideActorPtr)
	{
		if (InstanceData.GuideActorPtr->bWantToStop)
		{
			return EStateTreeRunStatus::Failed;
		}
	}
	else
	{
		return EStateTreeRunStatus::Failed;
	}

	return FStateTreeTaskBase::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_QuestInteractionBase::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	return Super::EnterState(Context, Transition);
}

const UStruct* FSTT_GuideInteractionNotify::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_GuideInteractionNotify::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	Super::EnterState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.GuideActorPtr)
	{
	}
	else
	{
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.GloabVariable->TaskNodeResuleHelper.TaskID = InstanceData.TaskID;

	UQuestSubSystem::GetInstance()->GetCurrentGuideThread()->AddEvent(InstanceData.GloabVariable->TaskNodeResuleHelper);

	return EStateTreeRunStatus::Succeeded;
}

EStateTreeRunStatus FSTT_GuideInteraction_GiveGuideThread::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	Super::EnterState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.GuideActorPtr)
	{
		if (InstanceData.BrandGuideThreadClass)
		{
			UQuestSubSystem::GetInstance()->AddGuideThread_Brand(InstanceData.BrandGuideThreadClass);
			if (InstanceData.bIsAutomaticActive)
			{
				UQuestSubSystem::GetInstance()->ActiveBrandGuideThread(InstanceData.BrandGuideThreadClass);
			}

			return EStateTreeRunStatus::Succeeded;
		}
		else if (InstanceData.ImmediateGuideThreadClass)
		{
			UQuestSubSystem::GetInstance()->StartParallelGuideThread(InstanceData.ImmediateGuideThreadClass);

			return EStateTreeRunStatus::Succeeded;
		}
	}
	else
	{
	}
	return EStateTreeRunStatus::Failed;
}
