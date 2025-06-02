#include "STT_GuideThreadBase.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"
#include "Kismet/KismetMathLibrary.h"

#include "GuideThreadBase.h"

const UStruct* FSTT_GuideThreadBase::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_GuideThreadBase::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
	) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
	                                                          *this
	                                                         );

	if (InstanceData.GuideThreadActorPtr)
	{
		InstanceData.GuideThreadActorPtr->UpdateCurrentTaskNode(
		                                                        GetTaskNodeDescripton(
			                                                         Context
			                                                        )
		                                                       );
	}
	else
	{
		return EStateTreeRunStatus::Failed;
	}

	return Super::EnterState(
	                         Context,
	                         Transition
	                        );
}

void FSTT_GuideThreadBase::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
	) const
{
	Super::ExitState(
	                 Context,
	                 Transition
	                );
}

FTaskNodeDescript FSTT_GuideThreadBase::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
	) const
{
	FTaskNodeDescript TaskNodeDescript;

	TaskNodeDescript.bIsFreshPreviouDescription = false;

	return TaskNodeDescript;
}

EStateTreeRunStatus FSTT_GuideThreadFail::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
	) const
{
	// TODO 任务失败时 

	checkNoEntry();

	return Super::EnterState(
	                         Context,
	                         Transition
	                        );
}
