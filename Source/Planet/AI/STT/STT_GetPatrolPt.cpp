
#include "STT_GetPatrolPt.h"

#include <NavigationSystem.h>

#include "AITask_DashToLeader.h"
#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "AITask_SwitchWalkState.h"
#include "STE_Human.h"
#include "Planet.h"

#ifdef WITH_EDITOR
static TAutoConsoleVariable<int32> DrawDebugSTT_GetPatrolPt(
	TEXT("Skill.DrawDebug.STT_GetPatrolPt"),
	1,
	TEXT("")
	TEXT(" default: 0"));
#endif

EStateTreeRunStatus FSTT_GetPatrolPt::EnterState(
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

	if (UNavigationSystemV1::K2_GetRandomReachablePointInRadius(
		InstanceData.TargetCharacterPtr,
		InstanceData.TargetCharacterPtr->GetActorLocation(),
		InstanceData.Location,
		800.f
	))
	{
#ifdef WITH_EDITOR
		if (DrawDebugSTT_GetPatrolPt.GetValueOnGameThread())
		{
			DrawDebugSphere(GetWorldImp(), InstanceData.Location, 20, 20, FColor::Yellow, false, 5);
		}
#endif

		return Super::EnterState(Context, Transition);
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Failed;
}
