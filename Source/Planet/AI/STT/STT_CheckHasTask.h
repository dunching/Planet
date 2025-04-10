// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateType.h"

#include "STT_CheckHasTask.generated.h"

class UEnvQuery;

class AHumanCharacter;
class AHumanCharacter_AI;
class AHumanAIController;
class UAITask_ReleaseSkill;
class USTE_AICharacterController;
class UGloabVariable;

USTRUCT()
struct PLANET_API FStateTreeSTT_CheckHasTaskTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter_AI> CharacterPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

};

USTRUCT()
struct PLANET_API FSTT_CheckHasTask : public FStateTreeAIActionTaskBase//FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeSTT_CheckHasTaskTaskInstanceData;

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

	EStateTreeRunStatus PerformGameplayTask(FStateTreeExecutionContext& Context)const;

};

USTRUCT()
struct PLANET_API FSTT_CheckHasTemopraryTask : public FStateTreeAIActionTaskBase//FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeSTT_CheckHasTaskTaskInstanceData;

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

	EStateTreeRunStatus PerformGameplayTask(FStateTreeExecutionContext& Context)const;

};
