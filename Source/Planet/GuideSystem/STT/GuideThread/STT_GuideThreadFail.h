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

#include "STT_GuideThreadFail.generated.h"

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
class UGameplayTask_Guide_AddToTarget;
class UGameplayTask_Guide_CollectResource;
class UGameplayTask_Guide_DefeatEnemy;

struct FConsumableProxy;
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

	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	protected:

	virtual FTaskNodeDescript GetTaskNodeDescripton(FStateTreeExecutionContext& Context) const;
	
};

#pragma region Generic
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
#pragma endregion

#pragma region Fail
USTRUCT()
struct PLANET_API FSTID_GuideThreadFail :
	public FSTID_GuideThreadBase
{
	GENERATED_BODY()

};

// 执行引导任务 失败时
USTRUCT()
struct PLANET_API FSTT_GuideThreadFail :
	public FSTT_GuideThreadBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
};
#pragma endregion

#pragma region DistributeRewards
USTRUCT()
struct PLANET_API FSTID_GuideThreadDistributeRewards :
	public FSTID_GuideThreadBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Param)
	FGuid RewardsItemID;
};

// 执行任务 获取奖励
USTRUCT()
struct PLANET_API FSTT_GuideThreadDistributeRewards :
	public FSTT_GuideThreadBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThreadDistributeRewards;

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
};
#pragma endregion

#pragma region Monologue
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
#pragma endregion

#pragma region WaitComplete
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
#pragma endregion

#pragma region CollectResource
USTRUCT()
struct PLANET_API FSTID_GuideThreadCollectResource :
	public FSTID_GuideThreadBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Param)
	FGameplayTag ResourceType;

	UPROPERTY(EditAnywhere, Category = Param)
	int32 Num = 1;

	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Guide_CollectResource> GameplayTaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

};

// 要求玩家采集指定类型的资源
USTRUCT()
struct PLANET_API FSTT_GuideThreadCollectResource :
	public FSTT_GuideThreadBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThreadCollectResource;

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
#pragma endregion

#pragma region DefeatEnemy
USTRUCT()
struct PLANET_API FSTID_GuideThreadDefeatEnemy :
	public FSTID_GuideThreadBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Param)
	FGameplayTag EnemyType;

	UPROPERTY(EditAnywhere, Category = Param)
	int32 Num = 1;

	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Guide_DefeatEnemy> GameplayTaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

};

// 要求玩家采集指定类型的资源
USTRUCT()
struct PLANET_API FSTT_GuideThreadDefeatEnemy :
	public FSTT_GuideThreadBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThreadDefeatEnemy;

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
#pragma endregion

#pragma region ConversationWithTarget
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
	
	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

protected:
	
	EStateTreeRunStatus PerformMoveTask(FStateTreeExecutionContext& Context) const;

	virtual FTaskNodeDescript GetTaskNodeDescripton(FStateTreeExecutionContext& Context) const override;
	
};
#pragma endregion

#pragma region AddInteractionToTarget
USTRUCT()
struct PLANET_API FSTID_GuideThreadAddInteractionToTarget :
	public FSTID_GuideThreadBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Param)
	FGuid CharacterID;
	
	UPROPERTY(EditAnywhere, Category = Param)
	TSubclassOf<AGuideInteractionActor> GuideInteractionActorClass;
	
	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Guide_AddToTarget> GameplayTaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
};

// 执行引导任务 给目标角色添加互动引导内容
USTRUCT()
struct PLANET_API FSTT_GuideThreadAddInteractionToTarget :
	public FSTT_GuideThreadBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThreadAddInteractionToTarget;

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
	
protected:
	
	EStateTreeRunStatus PerformMoveTask(FStateTreeExecutionContext& Context) const;

};
#pragma endregion
