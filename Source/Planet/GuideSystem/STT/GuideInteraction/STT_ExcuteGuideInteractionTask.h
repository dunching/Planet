// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateType.h"
#include "STT_ExcuteGuideTask.h"

#include "STT_ExcuteGuideInteractionTask.generated.h"

class AGuideActor;
class AGuideThread;
class AGuideMainThread;
class AGuideInteractionActor;
class UPAD_TaskNode_Guide;
class UPAD_TaskNode_Interaction;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;
class UGameplayTask_Base;

// 与 NPC交互的任务
USTRUCT()
struct PLANET_API FStateTreeExcuteGuideInteractionTaskBaseInstanceData :
	public FStateTreeExcuteGuideTaskTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AGuideInteractionActor> GuideActorPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter_Player> PlayerCharacterPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter> TargetCharacterPtr = nullptr;
};

USTRUCT()
struct PLANET_API FSTT_ExcuteGuideInteractionBaseTask :
	public FStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeExcuteGuideInteractionTaskBaseInstanceData;

	virtual const UStruct* GetInstanceDataType() const override;
};

// 与 NPC交互的任务
USTRUCT()
struct PLANET_API FStateTreeExcuteGuideInteractionTaskInstanceData :
	public FStateTreeExcuteGuideInteractionTaskBaseInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Param)
	TSoftObjectPtr<UPAD_TaskNode_Interaction> TaskNodeRef;

	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Base> GameplayTaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
};

USTRUCT()
struct PLANET_API FSTT_ExcuteGuideInteractionGenericTask :
	public FSTT_ExcuteGuideInteractionBaseTask
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeExcuteGuideInteractionTaskInstanceData;

	FSTT_ExcuteGuideInteractionGenericTask();

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

	virtual EStateTreeRunStatus PerformMoveTask(FStateTreeExecutionContext& Context) const;
};

// 与 NPC交互的任务 失败时
USTRUCT()
struct PLANET_API FSTT_ExcuteGuideInteractionFaileTask :
	public FSTT_ExcuteGuideInteractionBaseTask
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
};

USTRUCT()
struct PLANET_API FStateTreeExcuteGuideInteractionSelectTaskInstanceData :
	public FStateTreeExcuteGuideInteractionTaskBaseInstanceData
{
	GENERATED_BODY()


	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Base> GameplayTaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
	
	UPROPERTY(EditAnywhere, Category = Param)
	int32 Index = -1;
	
	// 
	UPROPERTY(EditAnywhere, Category = Param)
	FGuid NotifyTaskID;

	//结束时移除这个节点
	UPROPERTY(EditAnywhere, Category = Param)
	TSubclassOf<AGuideInteractionActor> GuideInteractionActorClass;
	
};

// 与 NPC交互的任务 选择的内容通知到引导任务
USTRUCT()
struct PLANET_API FSTT_ExcuteGuideInteractionSelectTask :
	public FSTT_ExcuteGuideInteractionBaseTask
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeExcuteGuideInteractionSelectTaskInstanceData;

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus PerformMoveTask(FStateTreeExecutionContext& Context) const;
};
