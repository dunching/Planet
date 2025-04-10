
#include "STT_ReleaseSkill.h"

#include <NavigationSystem.h>
#include <Components/CapsuleComponent.h>

#include "LogWriter.h"

#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "AITask_ReleaseSkill.h"
#include "STE_AICharacterController.h"

#ifdef WITH_EDITOR
static TAutoConsoleVariable<int32> Skill_DrawDebug_FSTT_ReleaseSkill(
	TEXT("Skill.DrawDebug.FSTT_ReleaseSkill"),
	0,
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

	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(
		InstanceData.AIControllerPtr->FindComponentByInterface(UGameplayTaskOwnerInterface::StaticClass())
		);
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.AIControllerPtr;
	}

	return PerformGameplayTask(Context);
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
		if (InstanceData.GloabVariable->bIsFarawayOriginal)
		{
			return EStateTreeRunStatus::Failed;
		}
		else if
			(
				InstanceData.GloabVariable->TargetCharacterPtr.IsValid() &&
				InstanceData.GloabVariable->TargetCharacterPtr->GetIsValidTarget()
				)
		{
			const auto Pt1 = InstanceData.GloabVariable->TargetCharacterPtr->GetActorLocation();
			const auto Pt2 = InstanceData.CharacterPtr->GetActorLocation();

			float OutRadius1 = InstanceData.GloabVariable->TargetCharacterPtr->GetCapsuleComponent()->GetScaledCapsuleRadius();
			float OutRadius2 = InstanceData.CharacterPtr->GetCapsuleComponent()->GetScaledCapsuleRadius();

			const auto Distance = FVector::Distance(Pt1, Pt2);

			if (Distance > (InstanceData.GloabVariable->QueryDistance + OutRadius1 + OutRadius2))
			{
				return EStateTreeRunStatus::Failed;
			}
		}
		else
		{
			return EStateTreeRunStatus::Failed;
		}
	}

	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_ReleaseSkill::PerformGameplayTask(FStateTreeExecutionContext& Context) const
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
