// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateType.h"

#include "STT_ExcuteTask.generated.h"

class UEnvQuery;

class AHumanCharacter;
class AHumanCharacter_AI;
class AHumanAIController;
class UAITask_ExcuteTask_Base;
class UAITask_ExcuteTemporaryTask_Base;
class UAITask_Conversation;
class USTE_AICharacterController;
class UGloabVariable;
class UPAD_TaskNode;
class UPAD_TaskNode_Preset;
class UTaskNode_Temporary;

USTRUCT()
struct PLANET_API FStateTreeExcuteTaskAutomaticInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter_AI> CharacterPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanAIController> AIControllerPtr = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAITask_ExcuteTask_Base> AITaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

	UPROPERTY(Transient)
	TArray<UPAD_TaskNode_Preset*> TaskNodesAry;

	UPROPERTY(Transient)
	UPAD_TaskNode_Preset* CurrentTaskNodePtr = nullptr;
};

/* 让AIController执行一些预置的任务，比如让AI说某一段对话、去到某一处地点
 * 相比于内置的任务逻辑，这个任务更倾向于执行由“任务系统（AWolrdProcess）”分发下来的任务
 */
USTRUCT()
struct PLANET_API FSTT_ExcuteTask_Automatic : public FStateTreeAIActionTaskBase //FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeExcuteTaskAutomaticInstanceData;

	using FAITaskType_Conversation = UAITask_Conversation;

	using FAITaskType_MoveToLocation = UAITask_Conversation;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

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

	EStateTreeRunStatus PerformMoveTask(FStateTreeExecutionContext& Context) const;
};


USTRUCT()
struct PLANET_API FStateTreeExcuteTemporaryTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter_AI> CharacterPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanAIController> AIControllerPtr = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAITask_ExcuteTemporaryTask_Base> AITaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

	UPROPERTY(Transient)
	TArray<UTaskNode_Temporary*> TaskNodesAry;

	UPROPERTY(Transient)
	UPAD_TaskNode_Preset* CurrentTaskNodePtr = nullptr;
};

/* 让AIController执行一些预置的任务，比如让AI说某一段对话、去到某一处地点
 * 相比于内置的任务逻辑，这个任务更倾向于执行由“任务系统（AWolrdProcess）”分发下来的任务
 */
USTRUCT()
struct PLANET_API FSTT_ExcuteTask_Temporary : public FStateTreeAIActionTaskBase //FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeExcuteTemporaryTaskInstanceData;

	using FAITaskType_Conversation = UAITask_Conversation;

	using FAITaskType_MoveToLocation = UAITask_Conversation;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

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

	EStateTreeRunStatus PerformMoveTask(FStateTreeExecutionContext& Context) const;
};
