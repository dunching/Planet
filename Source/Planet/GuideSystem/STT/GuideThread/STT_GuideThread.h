// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateType.h"
#include "GuideActor.h"
#include "STT_GuideBase.h"

#include "STT_GuideThread.generated.h"

class AGuideActor;
class AGuideThread;
class AGuideMainThread;
class AGuideInteractionActor;
class UPAD_TaskNode_Guide;
class UPAD_TaskNode_Interaction;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_AI;
class AHumanCharacter_Player;
class UGameplayTask_Base;
class UGameplayTask_Guide_WaitComplete;
class UGameplayTask_Guide_ConversationWithTarget;

struct FTaskNode_Conversation_SentenceInfo;

USTRUCT()
struct PLANET_API FSTID_GuideThreadBase :
	public FSTID_GuideBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AGuideThread> GuideActorPtr = nullptr;

	UPROPERTY(Transient)
	AHumanCharacter_Player* PlayerCharacterPtr = nullptr;

};

// 执行引导任务 
USTRUCT()
struct PLANET_API FSTT_GuideThreadBase :
	public FStateTreeTaskBase
{
	GENERATED_BODY()
	
	using FInstanceDataType = FSTID_GuideThreadBase;

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	protected:

	virtual FTaskNodeDescript GetTaskNodeDescripton(FStateTreeExecutionContext& Context) const;
	
};

USTRUCT()
struct PLANET_API FSTID_GuideThreadGeneric :
	public FSTID_GuideThreadBase
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

};

// 执行引导任务 
USTRUCT()
struct PLANET_API FSTT_GuideThreadGenraric :
	public FSTT_GuideThreadBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThreadGeneric;

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
struct PLANET_API FSTID_GuideThreadFaileTask :
	public FSTID_GuideThreadBase
{
	GENERATED_BODY()

};

// 执行引导任务 失败时
USTRUCT()
struct PLANET_API FSTT_GuideThread :
	public FSTT_GuideThreadBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
};

USTRUCT()
struct PLANET_API FSTID_GuideThreadMonologue :
	public FSTID_GuideThreadBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Param)
	TArray<FTaskNode_Conversation_SentenceInfo> ConversationsAry;

	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Base> GameplayTaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

};

// 执行引导任务 对话
USTRUCT()
struct PLANET_API FSTT_GuideThreadMonologue :
	public FSTT_GuideThreadBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThreadMonologue;

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

USTRUCT()
struct PLANET_API FSTID_GuideThreadWaitComplete :
	public FSTID_GuideThreadBase
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Guide_WaitComplete> GameplayTaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
};

// 等待完成
USTRUCT()
struct PLANET_API FSTT_GuideThreadWaitComplete :
	public FSTT_GuideThreadBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThreadWaitComplete;

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
	
};

USTRUCT()
struct PLANET_API FSTID_GuideThreadConversationWithTarget :
	public FSTID_GuideThreadBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Param)
	FGuid CharacterID;
	
	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Guide_ConversationWithTarget> GameplayTaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
};

// 执行引导任务 前往与目标NPC对话
USTRUCT()
struct PLANET_API FSTT_GuideThreadConversationWithTarget :
	public FSTT_GuideThreadBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThreadConversationWithTarget;

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
	
};
