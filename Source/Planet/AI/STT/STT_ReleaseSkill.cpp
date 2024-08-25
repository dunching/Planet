
#include "STT_ReleaseSkill.h"

#include <NavigationSystem.h>

#include "LogWriter.h"

#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "AITask_ReleaseSkill.h"

#ifdef WITH_EDITOR
static TAutoConsoleVariable<int32> Skill_DrawDebug_FSTT_ReleaseSkill(
	TEXT("Skill.DrawDebug.FSTT_ReleaseSkill"),
	1,
	TEXT("")
	TEXT(" default: 0"));
#endif

EStateTreeRunStatus FSTT_ReleaseSkill::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
)const
{
#ifdef WITH_EDITOR
	if (Skill_DrawDebug_FSTT_ReleaseSkill.GetValueOnGameThread())
	{
		PRINTFUNC();
	}
#endif

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
	InstanceData.GloabVariable = NewObject<UReleaseSkillGloabVariable>();

	return PerformMoveTask(Context);
}

void FSTT_ReleaseSkill::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
)const
{
#ifdef WITH_EDITOR
	if (Skill_DrawDebug_FSTT_ReleaseSkill.GetValueOnGameThread())
	{
		PRINTFUNC();
	}
#endif

	Super::ExitState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.AITaskPtr && InstanceData.AITaskPtr->GetState() != EGameplayTaskState::Finished)
	{
		InstanceData.AITaskPtr->ExternalCancel();
	}
	InstanceData.AITaskPtr = nullptr;
}

EStateTreeRunStatus FSTT_ReleaseSkill::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.AITaskPtr && InstanceData.AITaskPtr->GetState() != EGameplayTaskState::Finished)
	{
		InstanceData.AITaskPtr->bIsPauseRelease = !InstanceData.GloabVariable->bIsNeedRelease;
	}

	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_ReleaseSkill::PerformMoveTask(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.AITaskPtr)
	{
		InstanceData.AITaskPtr =
			UAITask::NewAITask<FAITaskType>(*InstanceData.AIControllerPtr, *InstanceData.TaskOwner);
	}

	if (InstanceData.AITaskPtr)
	{
		InstanceData.AITaskPtr->SetUp(InstanceData.CharacterPtr.Get());

		if (InstanceData.AITaskPtr->IsActive())
		{
			InstanceData.AITaskPtr->ConditionalPerformTask();
		}
		else
		{
			InstanceData.AITaskPtr->ReadyForActivation();
		}
	}
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_UpdateReleaseSkillStuta::EnterState(
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

	if (InstanceData.GloabVariable)
	{
		switch (InstanceData.CurrentState)
		{
		case EUpdateReleaseSkillStuteType::kCheck:
		{
			InstanceData.GloabVariable->bIsNeedRelease =
				InstanceData.GloabVariable->UpdateReleaseSkillStuteType == EUpdateReleaseSkillStuteType::kRelease;
		}
		break;
		case EUpdateReleaseSkillStuteType::kMoveTo:
		{
			const auto Distance = FVector::Distance(InstanceData.TargetLocation, InstanceData.CharacterPtr->GetActorLocation());
			InstanceData.GloabVariable->bIsNeedRelease =
				(Distance < (InstanceData.AcceptableRadius + 100.f));
		}
		break;
		case EUpdateReleaseSkillStuteType::kRelease:
		{
		}
		break;
		}
		InstanceData.GloabVariable->UpdateReleaseSkillStuteType = InstanceData.CurrentState;
	}

	return EStateTreeRunStatus::Succeeded;
}
