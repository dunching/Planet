#include "STT_IsNearLeader.h"

#include "HumanCharacter_AI.h"
#include "STE_CharacterBase.h"

inline EStateTreeRunStatus FSTT_IsNearLeader::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
	) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.CharacterPtr)
	{
		return EStateTreeRunStatus::Failed;
	}

	const auto Distance = FVector::Dist2D(
	                                      InstanceData.CharacterPtr->GetActorLocation(),
	                                      InstanceData.GloabVariable->Location
	                                     );
	if (InstanceData.bIsNearLeader)
	{
		return Distance < InstanceData.Distance ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Failed;
	}
	else
	{
		return Distance > InstanceData.Distance ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Failed;
	}
}
