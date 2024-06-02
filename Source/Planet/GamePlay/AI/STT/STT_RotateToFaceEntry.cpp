
#include "STT_RotateToFaceEntry.h"

#include <NavigationSystem.h>
#include <EnvironmentQuery/EnvQueryManager.h>

#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "AITask_ReleaseSkill.h"
#include "STE_Human.h"
#include "Planet.h"

namespace STT_RotateToFaceEntry
{
	FName Donut_InnerRadius = TEXT("Donut.InnerRadius");

	FName Donut_OuterRadius = TEXT("Donut.OuterRadius");
}

FSTT_RotateToFaceEntry::FSTT_RotateToFaceEntry() :
	Super()
{
}

const UStruct* FSTT_RotateToFaceEntry::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_RotateToFaceEntry::EnterState(
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

	return PerformMoveTask(Context);
}

void FSTT_RotateToFaceEntry::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
)const
{
	Super::ExitState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	UEnvQueryManager* QueryManager = UEnvQueryManager::GetCurrent(InstanceData.CharacterPtr);

	if (QueryManager)
	{
		QueryManager->AbortQuery(InstanceData.RequestID);
		InstanceData.Reset();
	}
}

EStateTreeRunStatus FSTT_RotateToFaceEntry::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.bIsFinished)
	{
		if (InstanceData.ResultSPtr)
		{
			bool bSuccess = InstanceData.ResultSPtr->IsSuccessful() && (InstanceData.ResultSPtr->Items.Num() >= 1);
			if (bSuccess)
			{
				InstanceData.GloabVariable->Location = InstanceData.ResultSPtr->GetItemAsLocation(0);
				InstanceData.Location = InstanceData.GloabVariable->Location;

#if WITH_EDITOR
				DrawDebugSphere(GetWorldImp(), InstanceData.Location, 20, 20, FColor::Yellow, false, 5);
#endif

				return EStateTreeRunStatus::Succeeded;
			}
		}
		return EStateTreeRunStatus::Failed;
	}

	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_RotateToFaceEntry::PerformMoveTask(FStateTreeExecutionContext& Context)const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.QueryTemplate.IsNull())
	{
		FEnvQueryRequest QueryRequest(InstanceData.QueryTemplate, InstanceData.CharacterPtr);

		// SetIntParam <-这是在干啥？
		QueryRequest.SetFloatParam(STT_RotateToFaceEntry::Donut_OuterRadius, InstanceData.Donut_OuterRadius);

		auto QueryFinishedDelegate = FQueryFinishedSignature::CreateRaw(&InstanceData, &FInstanceDataType::OnQueryFinished);
		InstanceData.RequestID = QueryRequest.Execute(InstanceData.RunMode, QueryFinishedDelegate);

		return EStateTreeRunStatus::Running;
	}
	return EStateTreeRunStatus::Failed;
}

void FStateTreeRotateToFaceEntryTaskInstanceData::OnQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	bIsFinished = true;

	if (Result->IsAborted())
	{
		return;
	}

	ResultSPtr = Result;
}

void FStateTreeRotateToFaceEntryTaskInstanceData::Reset()
{
	RequestID = 0;
	bIsFinished = false;
	ResultSPtr.Reset();
}
