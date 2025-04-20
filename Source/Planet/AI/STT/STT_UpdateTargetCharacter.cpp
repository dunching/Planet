#include "STT_UpdateTargetCharacter.h"

#include <NavigationSystem.h>

#include "AITask_DashToLeader.h"
#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "AITask_SwitchWalkState.h"
#include "STE_AICharacterController.h"
#include "GroupManagger.h"
#include "HumanCharacter_AI.h"

EStateTreeRunStatus FSTT_UpdateTargetCharacter::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.CharacterPtr)
	{
		return EStateTreeRunStatus::Failed;
	}

	if (InstanceData.bIsCcontinuous)
	{
	}
	else
	{
		PerformGameplayTask(Context);
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::EnterState(Context, Transition);
}

EStateTreeRunStatus FSTT_UpdateTargetCharacter::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	Super::Tick(Context, DeltaTime);

	return PerformGameplayTask(Context);
}

EStateTreeRunStatus FSTT_UpdateTargetCharacter::PerformGameplayTask(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(
		InstanceData.AIControllerPtr->FindComponentByInterface(UGameplayTaskOwnerInterface::StaticClass())
	);
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.AIControllerPtr;
	}

	const auto GetKnowCharater = InstanceData.CharacterPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->
	                                          GetKnowCharater();
	if (GetKnowCharater.IsValidIndex(0))
	{
		InstanceData.GloabVariable->TargetCharacterPtr = GetKnowCharater[0];
	}
	else
	{
		InstanceData.GloabVariable->TargetCharacterPtr = nullptr;
	}

	if (InstanceData.bIsCcontinuous)
	{
		if (InstanceData.bCheckHave)
		{
			return InstanceData.GloabVariable->TargetCharacterPtr.IsValid() ?
				       EStateTreeRunStatus::Running :
				       EStateTreeRunStatus::Succeeded;
		}
		else
		{
			return !InstanceData.GloabVariable->TargetCharacterPtr.IsValid() ?
				       EStateTreeRunStatus::Running :
				       EStateTreeRunStatus::Succeeded;
		}
	}
	else
	{
	}

	return EStateTreeRunStatus::Running;
}
