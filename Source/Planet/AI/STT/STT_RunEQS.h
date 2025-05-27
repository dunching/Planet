// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateTypes.h"

#include "STT_RunEQS.generated.h"

class UEnvQuery;

class AHumanCharacter;
class AHumanAIController;
class UAITask_ReleaseSkill;
class USTE_Assistance;
class UGloabVariable_Character;

USTRUCT()
struct PLANET_API FStateTreeRunEQSTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter> CharacterPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanAIController> AIControllerPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	UGloabVariable_Character* GloabVariable = nullptr;

	UPROPERTY(EditAnywhere, Category = Param)
	TObjectPtr<UEnvQuery> QueryTemplate;

	UPROPERTY(EditAnywhere, Category = Param)
	TEnumAsByte<EEnvQueryRunMode::Type> RunMode = EEnvQueryRunMode::SingleResult;

	UPROPERTY(EditAnywhere, Category = Param)
	int32 Donut_InnerRadius = 150;

	UPROPERTY(EditAnywhere, Category = Param)
	int32 Donut_OuterRadius = 850;
	
	UPROPERTY(EditAnywhere, Category = Param)
	bool bRunForever = true;
	
	int32 RequestID = INDEX_NONE;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

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

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus PerformGameplayTask(FStateTreeExecutionContext& Context)const;

};
