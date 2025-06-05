#include "STT_UpdateTargetCharacter.h"

#include <NavigationSystem.h>

#include "AITask_DashToLeader.h"
#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "AITask_SwitchWalkState.h"
#include "STE_Assistance.h"
#include "GroupManagger.h"
#include "HumanCharacter_AI.h"
#include "STE_CharacterBase.h"

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

	if (InstanceData.bRunForever)
	{
	}
	else
	{
		return PerformGameplayTask(Context);
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

	UpdateTargetCharacter(Context);

	auto CharacterStateComponentPtr = InstanceData.CharacterPtr->GetCharacterNPCStateProcessorComponent();
	const auto GetKnowCharater = CharacterStateComponentPtr->GetTargetCharactersAry();
	if (GetKnowCharater.IsValidIndex(0))
	{
		InstanceData.GloabVariable_Character->TargetCharacterPtr = GetKnowCharater[0];
	}
	else
	{
		InstanceData.GloabVariable_Character->TargetCharacterPtr = nullptr;
	}

	if (InstanceData.bRunForever)
	{
		if (InstanceData.bCheckHave)
		{
			return InstanceData.GloabVariable_Character->TargetCharacterPtr.IsValid() ?
				       EStateTreeRunStatus::Running :
				       EStateTreeRunStatus::Succeeded;
		}
		else
		{
			return !InstanceData.GloabVariable_Character->TargetCharacterPtr.IsValid() ?
				       EStateTreeRunStatus::Running :
				       EStateTreeRunStatus::Succeeded;
		}
	}
	return EStateTreeRunStatus::Succeeded;
}

void FSTT_UpdateTargetCharacter::UpdateTargetCharacter(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.GloabVariable_Character->UpdateTargetCharacterFunc)
	{
		auto CharacterPtr = InstanceData.CharacterPtr;
		CharacterPtr->GetCharacterNPCStateProcessorComponent()->SetTargetCharactersAry(
			InstanceData.GloabVariable_Character->
			             UpdateTargetCharacterFunc()
		);
	}
}
