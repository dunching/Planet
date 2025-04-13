
#include "STT_Teleport.h"

#include <NavigationSystem.h>

#include "AITask_DashToLeader.h"
#include "GroupManagger.h"
#include "HumanAIController.h"
#include "HumanCharacter.h"

EStateTreeRunStatus FSTT_Teleport::EnterState(
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

	auto TeamHelperSPtr = InstanceData.AIControllerPtr->GetGroupSharedInfo()->GetTeamMatesHelperComponent();
	if (TeamHelperSPtr)
	{
		auto LeaderCharacterPtr = TeamHelperSPtr->GetOwnerCharacterProxyPtr()->GetCharacterActor().Get();
		FVector Destination = FVector::ZeroVector;
		UNavigationSystemV1::K2_GetRandomReachablePointInRadius(
			InstanceData.AIControllerPtr, LeaderCharacterPtr->GetActorLocation(), Destination, InstanceData.AcceptableRadius
			);

		if (InstanceData.CharacterPtr->TeleportTo(Destination,FRotator::ZeroRotator ))
		{
			return EStateTreeRunStatus::Succeeded;
		}

	}
	return EStateTreeRunStatus::Failed;
}
