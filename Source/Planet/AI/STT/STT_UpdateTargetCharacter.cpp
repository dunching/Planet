
#include "STT_UpdateTargetCharacter.h"

#include <NavigationSystem.h>

#include "AITask_DashToLeader.h"
#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "AITask_SwitchWalkState.h"
#include "STE_Human.h"

EStateTreeRunStatus FSTT_UpdateTargetCharacter::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
)const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.CharacterPtr)
	{
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(InstanceData.AIControllerPtr->FindComponentByInterface(UGameplayTaskOwnerInterface::StaticClass()));
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.AIControllerPtr;
	}

	InstanceData.GloabVariable->TargetCharacterPtr =
		InstanceData.CharacterPtr->GetGroupMnaggerComponent()->GetTeamHelper()->GetKnowCharacter();

	return EStateTreeRunStatus::Succeeded;
}
