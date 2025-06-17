#include "STT_SwitchWalkState.h"

#include <NavigationSystem.h>

#include "AITask_DashToLeader.h"
#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "AITask_SwitchWalkState.h"
#include "CharacterAbilitySystemComponent.h"
#include "HumanCharacter_AI.h"

EStateTreeRunStatus FSTT_SwitchWalkState::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.CharacterPtr)
	{
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(
		InstanceData.AIControllerPtr->FindComponentByInterface(UGameplayTaskOwnerInterface::StaticClass())
	);
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.AIControllerPtr;
	}

	InstanceData.CharacterPtr->GetCharacterAbilitySystemComponent()->SwitchWalkState(InstanceData.bIsRun);

	if (InstanceData.bRunForever)
	{
		return EStateTreeRunStatus::Running;
	}
	return EStateTreeRunStatus::Succeeded;
}

void FSTT_SwitchWalkState::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	InstanceData.CharacterPtr->GetCharacterAbilitySystemComponent()->SwitchWalkState(false);

	Super::ExitState(
		Context,
		Transition
	);
}
