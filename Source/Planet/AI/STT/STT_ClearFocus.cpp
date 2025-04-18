
#include "STT_ClearFocus.h"

#include "AIComponent.h"
#include "ResourceBoxStateTreeComponent.h"
#include "HumanCharacter_AI.h"
#include "StateProcessorComponent.h"

EStateTreeRunStatus FSTT_ClearFocus::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.CharacterPtr->GetStateProcessorComponent()->ClearFocusCharactersAry();
	
	return EStateTreeRunStatus::Succeeded;
}
