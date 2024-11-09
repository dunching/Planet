
#include "STT_UpdateQueryDistance.h"

#include <NavigationSystem.h>

#include "AITask_DashToLeader.h"
#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "AITask_SwitchWalkState.h"
#include "STE_Human.h"
#include "ProxyProcessComponent.h"

EStateTreeRunStatus FSTT_UpdateQueryDistance::EnterState(
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

	return Super::EnterState(Context, Transition);
}

EStateTreeRunStatus FSTT_UpdateQueryDistance::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.GloabVariable->QueryDistance =
		InstanceData.CharacterPtr->GetProxyProcessComponent()->GetCurrentWeaponAttackDistance();

	return Super::Tick(Context, DeltaTime);
}
