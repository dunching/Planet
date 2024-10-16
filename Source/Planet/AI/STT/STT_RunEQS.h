// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateType.h"

#include "STT_RunEQS.generated.h"

class UEnvQuery;

class AHumanCharacter;
class AHumanAIController;
class UAITask_ReleaseSkill;
class USTE_Human;
class UGloabVariable;

USTRUCT()
struct PLANET_API FStateTreeRunEQSTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter> CharacterPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanAIController> AIControllerPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	UGloabVariable* GloabVariable = nullptr;

	UPROPERTY(EditAnywhere, Category = Output)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = Param)
	TObjectPtr<UEnvQuery> QueryTemplate;

	UPROPERTY(EditAnywhere, Category = Param)
	TEnumAsByte<EEnvQueryRunMode::Type> RunMode = EEnvQueryRunMode::SingleResult;

	UPROPERTY(EditAnywhere, Category = Param)
	int32 Donut_InnerRadius = 150;

	UPROPERTY(EditAnywhere, Category = Param)
	int32 Donut_OuterRadius = 850;
	
	int32 RequestID = 0;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

	UPROPERTY(Transient)
	bool bIsFinished = false;

	TSharedPtr<FEnvQueryResult> ResultSPtr;

	void OnQueryFinished(TSharedPtr<FEnvQueryResult> Result);

	void Reset();

};

USTRUCT()
struct PLANET_API FSTT_RunEQS : public FStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeRunEQSTaskInstanceData;

	using FAITaskType = UAITask_ReleaseSkill;

	FSTT_RunEQS();

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

	virtual EStateTreeRunStatus PerformMoveTask(FStateTreeExecutionContext& Context)const;

};
