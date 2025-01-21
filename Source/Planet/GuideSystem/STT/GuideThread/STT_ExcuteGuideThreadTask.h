// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateType.h"
#include "GuideActor.h"
#include "STT_ExcuteGuideTask.h"

#include "STT_ExcuteGuideThreadTask.generated.h"

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

struct FTaskNode_Conversation_SentenceInfo;

USTRUCT()
struct PLANET_API FStateTreeExcuteGuideThreadBaseTaskInstanceData :
	public FStateTreeExcuteGuideTaskTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AGuideThread> GuideActorPtr = nullptr;

};

// 执行引导任务 
USTRUCT()
struct PLANET_API FSTT_ExcuteGuideThreadBaseTask :
	public FStateTreeTaskBase
{
	GENERATED_BODY()
	
	using FInstanceDataType = FStateTreeExcuteGuideThreadBaseTaskInstanceData;

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	protected:

	virtual FTaskNodeDescript GetTaskNodeDescripton(FStateTreeExecutionContext& Context) const;
	
};

USTRUCT()
struct PLANET_API FStateTreeExcuteGuideThreadGenericTaskInstanceData :
	public FStateTreeExcuteGuideThreadBaseTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Param)
	TSoftObjectPtr<UPAD_TaskNode_Guide> TaskNodeRef;

	UPROPERTY(EditAnywhere, Category = Param)
	FString Name;
	
	UPROPERTY(EditAnywhere, Category = Param)
	FString Description;
	
	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Base> GameplayTaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

	UPROPERTY(Transient)
	AHumanCharacter_Player* PlayerCharacterPtr = nullptr;
};

// 执行引导任务 
USTRUCT()
struct PLANET_API FSTT_ExcuteGuideThreadGenraricTask :
	public FSTT_ExcuteGuideThreadBaseTask
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeExcuteGuideThreadGenericTaskInstanceData;

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

protected:

	EStateTreeRunStatus PerformMoveTask(FStateTreeExecutionContext& Context) const;
	
	virtual FTaskNodeDescript GetTaskNodeDescripton(FStateTreeExecutionContext& Context) const override;
	
};

USTRUCT()
struct PLANET_API FStateTreeExcuteGuideThreadFaileTaskInstanceData :
	public FStateTreeExcuteGuideThreadBaseTaskInstanceData
{
	GENERATED_BODY()

};

// 执行引导任务 失败时
USTRUCT()
struct PLANET_API FSTT_ExcuteGuideThreadFaileTask :
	public FSTT_ExcuteGuideThreadBaseTask
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
};

USTRUCT()
struct PLANET_API FStateTreeExcuteGuideThreadMonologueTaskInstanceData :
	public FStateTreeExcuteGuideThreadBaseTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Param)
	TArray<FTaskNode_Conversation_SentenceInfo> ConversationsAry;

	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Base> GameplayTaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

	UPROPERTY(Transient)
	AHumanCharacter_Player* PlayerCharacterPtr = nullptr;
};

// 执行引导任务 对话
USTRUCT()
struct PLANET_API FSTT_ExcuteGuideThreadMonologueTask :
	public FSTT_ExcuteGuideThreadBaseTask
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeExcuteGuideThreadMonologueTaskInstanceData;

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
	
	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

protected:
	
	EStateTreeRunStatus PerformMoveTask(FStateTreeExecutionContext& Context) const;

	virtual FTaskNodeDescript GetTaskNodeDescripton(FStateTreeExecutionContext& Context) const override;
	
	float RemainingTime = 0.f;

	int32 SentenceIndex = 0;
};
