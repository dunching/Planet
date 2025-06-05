#include "STT_GetOriginalLocation.h"

#include "AITask_CharacterMonologue.h"
#include "STE_CharacterBase.h"

EStateTreeRunStatus FSTT_GetOriginalLocation::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
	) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.GloabVariable->bEQSSucessed = true;
	InstanceData.GloabVariable->Location = InstanceData.GloabVariable->OriginalLocation;

	return EStateTreeRunStatus::Succeeded;
}
