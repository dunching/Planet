#include "STT_UpdateQueryDistance.h"

#include <NavigationSystem.h>

#include "AITask_DashToLeader.h"
#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "AITask_SwitchWalkState.h"
#include "HumanCharacter_AI.h"
#include "STE_Assistance.h"
#include "ProxyProcessComponent.h"
#include "STE_CharacterBase.h"

EStateTreeRunStatus FSTT_UpdateQueryDistance::EnterState(
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

	if (InstanceData.bRunForever)
	{
		return Super::EnterState(Context, Transition);
	}
	else
	{
		PerformAction(Context);
		return EStateTreeRunStatus::Succeeded;
	}
}

EStateTreeRunStatus FSTT_UpdateQueryDistance::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	PerformAction(Context);

	return Super::Tick(Context, DeltaTime);
}

void FSTT_UpdateQueryDistance::PerformAction(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.GloabVariable->QueryDistance =
		InstanceData.CharacterPtr->GetProxyProcessComponent()->GetCurrentWeaponAttackDistance();
}
