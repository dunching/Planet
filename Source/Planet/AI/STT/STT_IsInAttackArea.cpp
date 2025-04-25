#include "STT_IsInAttackArea.h"

#include <NavigationSystem.h>
#include "Components/CapsuleComponent.h"

#include "AITask_DashToLeader.h"
#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "AITask_SwitchWalkState.h"
#include "HumanCharacter_AI.h"
#include "STE_Assistance.h"
#include "STE_CharacterBase.h"

EStateTreeRunStatus FSTT_IsInAttackArea::EnterState(
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

EStateTreeRunStatus FSTT_IsInAttackArea::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	PerformAction(Context);
	
	if (InstanceData.bIsFarawayOriginal)
	{
		return Super::Tick(Context, DeltaTime);
	}
	
	return EStateTreeRunStatus::Failed;
}

void FSTT_IsInAttackArea::PerformAction(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	if(InstanceData.GloabVariable->TargetCharacterPtr.IsValid())
	{
		float OutRadius1 = InstanceData.GloabVariable->TargetCharacterPtr->GetCapsuleComponent()->GetScaledCapsuleRadius();
		float OutRadius2 = InstanceData.CharacterPtr->GetCapsuleComponent()->GetScaledCapsuleRadius();

		const auto Distance =InstanceData.GloabVariable->QueryDistance + OutRadius1 + OutRadius2;
		InstanceData.bIsFarawayOriginal = FVector::Distance(
			InstanceData.GloabVariable->TargetCharacterPtr->GetActorLocation(),
			InstanceData.CharacterPtr->GetActorLocation()
		) <= Distance;
	}
	else
	{
		InstanceData.bIsFarawayOriginal = false;
	}
}
