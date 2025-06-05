#include "STT_UpdateBrandPromt.h"

#include <NavigationSystem.h>

#include "AIComponent.h"
#include "AITask_DashToLeader.h"
#include "GroupManagger.h"
#include "TaskPromt.h"
#include "HumanCharacter.h"
#include "HumanCharacter_AI.h"
#include "PlanetGameViewportClient.h"

EStateTreeRunStatus FSTT_UpdateBrandPromt::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.CharacterPtr)
	{
		return EStateTreeRunStatus::Failed;
	}

	const auto InteractionLists = InstanceData.CharacterPtr->GetSceneActorInteractionComponent()->
	                                           GetGuideInteractionAry();

	for (const auto& Iter : InteractionLists)
	{
		if (Iter.bIsTask && Iter.bIsEnable)
		{
			InstanceData.bHaveTask = true;
			InstanceData.CharacterPtr->GetAIComponent()->DisplayTaskPromy(InstanceData.TaskPromtClass);
			return EStateTreeRunStatus::Succeeded;
		}
	}

	InstanceData.bHaveTask = false; 
	InstanceData.CharacterPtr->GetAIComponent()->StopDisplayTaskPromy();
	return EStateTreeRunStatus::Succeeded;
}

void FSTT_UpdateBrandPromt::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	Super::ExitState(Context, Transition);
}
