
#include "STT_RunEQS.h"

#include <NavigationSystem.h>
#include <EnvironmentQuery/EnvQueryManager.h>

#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "AITask_ReleaseSkill.h"
#include "STE_Human.h"

namespace STT_RunEQS
{
	FName Donut_InnerRadius = TEXT("Donut.InnerRadius");

	FName Donut_OuterRadius = TEXT("Donut.OuterRadius");
}

FSTT_RunEQS::FSTT_RunEQS():
	Super()
{
}

EStateTreeRunStatus FSTT_RunEQS::EnterState(
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

void FSTT_RunEQS::ExitState(
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
		InstanceData.RequestID = 0;
		InstanceData.ResultSPtr.Reset();
	}
}

EStateTreeRunStatus FSTT_RunEQS::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
	) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	const bool bValid = (InstanceData.RequestID >= 0);
	if (InstanceData.ResultSPtr && bValid)
	{
		bool bSuccess = InstanceData.ResultSPtr->IsSuccessful() && (InstanceData.ResultSPtr->Items.Num() >= 1);
		if (bSuccess)
		{
			InstanceData.Location = InstanceData.Location;

			return EStateTreeRunStatus::Succeeded;
		}
		else
		{
			return EStateTreeRunStatus::Failed;
		}
	}
	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_RunEQS::PerformMoveTask(FStateTreeExecutionContext& Context)const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.QueryTemplate.IsNull())
	{
		FEnvQueryRequest QueryRequest(InstanceData.QueryTemplate, InstanceData.CharacterPtr);

		QueryRequest.SetIntParam(STT_RunEQS::Donut_OuterRadius, InstanceData.Donut_OuterRadius);

		auto QueryFinishedDelegate = FQueryFinishedSignature::CreateRaw(&InstanceData, &FInstanceDataType::OnQueryFinished);
		InstanceData.RequestID = QueryRequest.Execute(InstanceData.RunMode, QueryFinishedDelegate);

		return EStateTreeRunStatus::Running;
	}
	return EStateTreeRunStatus::Failed;
}

void FStateTreeRunEQSTaskInstanceData::OnQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	if (Result->IsAborted())
	{
		return;
	}

	bool bSuccess = Result->IsSuccessful() && (Result->Items.Num() >= 1);
	if (bSuccess)
	{
		GloabVariable->Location = Result->GetItemAsLocation(0);
		ResultSPtr = Result;
	}
}
