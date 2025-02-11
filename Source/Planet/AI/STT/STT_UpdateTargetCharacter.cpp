
#include "STT_UpdateTargetCharacter.h"

#include <NavigationSystem.h>

#include "AITask_DashToLeader.h"
#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "AITask_SwitchWalkState.h"
#include "STE_AICharacterController.h"
#include "GroupManagger.h"

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

	return Super::EnterState(Context, Transition);
}

EStateTreeRunStatus FSTT_UpdateTargetCharacter::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	PerformMoveTask(Context);

	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_UpdateTargetCharacter::PerformMoveTask(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(InstanceData.AIControllerPtr->FindComponentByInterface(UGameplayTaskOwnerInterface::StaticClass()));
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.AIControllerPtr;
	}

	InstanceData.GloabVariable->TargetCharacterPtr =
		InstanceData.CharacterPtr->GetGroupSharedInfo()->GetTeamMatesHelperComponent()->GetKnowCharacter();

	return EStateTreeRunStatus::Running;
}
