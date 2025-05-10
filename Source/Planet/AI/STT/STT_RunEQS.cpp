
#include "STT_RunEQS.h"

#include <NavigationSystem.h>
#include <EnvironmentQuery/EnvQueryManager.h>

#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "AITask_ReleaseSkill.h"
#include "STE_Assistance.h"
#include "Planet.h"
#include "STE_CharacterBase.h"

#ifdef WITH_EDITOR
static TAutoConsoleVariable<int32> DrawDebugSTT_RunEQS(
	TEXT("Skill.DrawDebug.STT_RunEQS"),
	0,
	TEXT("")
	TEXT(" default: 0"));
#endif

namespace STT_RunEQS
{
	FName Donut_InnerRadius = TEXT("Donut.InnerRadius");

	FName Donut_OuterRadius = TEXT("Donut.OuterRadius");
}

FSTT_RunEQS::FSTT_RunEQS() :
	Super()
{
}

const UStruct* FSTT_RunEQS::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
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

	return Super::EnterState(Context, Transition);
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
		InstanceData.Reset();
	}
}

EStateTreeRunStatus FSTT_RunEQS::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.GloabVariable->bEQSSucessed)
	{
		if (InstanceData.ResultSPtr)
		{
			bool bSuccess = InstanceData.ResultSPtr->IsSuccessful() && (InstanceData.ResultSPtr->Items.Num() >= 1);
			if (bSuccess)
			{
				InstanceData.GloabVariable->Location = InstanceData.ResultSPtr->GetItemAsLocation(0);

#ifdef WITH_EDITOR
				if (DrawDebugSTT_RunEQS.GetValueOnGameThread())
				{
					DrawDebugSphere(GetWorldImp(), InstanceData.GloabVariable->Location, 20, 20, FColor::Yellow, false, 5);
				}
#endif

				return Super::Tick(Context, DeltaTime);
			}
		}
		return EStateTreeRunStatus::Failed;
	}
	else
	{
		if (InstanceData.RequestID <= INDEX_NONE)
		{
			return PerformGameplayTask(Context); 
		}
	}
	
	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_RunEQS::PerformGameplayTask(FStateTreeExecutionContext& Context)const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.QueryTemplate)
	{
		FEnvQueryRequest QueryRequest(InstanceData.QueryTemplate, InstanceData.CharacterPtr);

		// 在状态树里通过参数设置不生效？原因还不清楚 在这里通过这种方式设置 ↓
		// SetIntParam <-这是在干啥？
		QueryRequest.SetFloatParam(STT_RunEQS::Donut_InnerRadius, InstanceData.Donut_InnerRadius);
		QueryRequest.SetFloatParam(STT_RunEQS::Donut_OuterRadius, InstanceData.Donut_OuterRadius);

		auto QueryFinishedDelegate = FQueryFinishedSignature::CreateRaw(&InstanceData, &FInstanceDataType::OnQueryFinished);
		InstanceData.RequestID = QueryRequest.Execute(InstanceData.RunMode, QueryFinishedDelegate);

		return EStateTreeRunStatus::Running;
	}
	return EStateTreeRunStatus::Failed;
}

void FStateTreeRunEQSTaskInstanceData::OnQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	GloabVariable->bEQSSucessed = true;
	RequestID = INDEX_NONE;

	if (Result->IsAborted())
	{
		return;
	}

	ResultSPtr = Result;
}

void FStateTreeRunEQSTaskInstanceData::Reset()
{
	RequestID = INDEX_NONE;
	GloabVariable->bEQSSucessed = false;
	ResultSPtr.Reset();
}
