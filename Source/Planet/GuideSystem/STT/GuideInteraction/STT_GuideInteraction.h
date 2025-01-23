// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateType.h"
#include "STT_GuideBase.h"
#include "TaskNode.h"
#include "GuideActor.h"

#include "STT_GuideInteraction.generated.h"

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
class UGameplayTask_Interaction_Conversation;
class UGameplayTask_Interaction_Option;
class UGameplayTask_Interaction_NotifyGuideThread;

// 与 NPC交互的任务 基类
USTRUCT()
struct PLANET_API FSTID_GuideInteractionTaskBase :
	public FSTID_GuideBase
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
struct PLANET_API FSTT_GuideInteractionBase :
	public FStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideInteractionTaskBase;

	virtual const UStruct* GetInstanceDataType() const override;
	
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

};

// 与 NPC交互的任务
USTRUCT()
struct PLANET_API FSTID_GuideInteractionGeneric :
	public FSTID_GuideInteractionTaskBase
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
struct PLANET_API FSTT_GuideInteractionGeneric :
	public FSTT_GuideInteractionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideInteractionGeneric;

	FSTT_GuideInteractionGeneric();

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

	EStateTreeRunStatus PerformMoveTask(FStateTreeExecutionContext& Context) const;
};

// 与 NPC交互的任务 失败时
USTRUCT()
struct PLANET_API FSTT_GuideInteractionFaile :
	public FSTT_GuideInteractionBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
};

USTRUCT()
struct PLANET_API FSTID_GuideInteractionNotify :
	public FSTID_GuideInteractionTaskBase
{
	GENERATED_BODY()


	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Interaction_NotifyGuideThread> GameplayTaskPtr = nullptr;

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
struct PLANET_API FSTT_GuideInteractionNotify :
	public FSTT_GuideInteractionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideInteractionNotify;

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
	
	EStateTreeRunStatus PerformMoveTask(FStateTreeExecutionContext& Context) const;
};

USTRUCT()
struct PLANET_API FSTID_GuideInteractionConversation :
	public FSTID_GuideInteractionTaskBase
{
	GENERATED_BODY()


	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Interaction_Conversation> GameplayTaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
	
	UPROPERTY(EditAnywhere, Category = Param)
	TArray<FTaskNode_Conversation_SentenceInfo> ConversationsAry;
	
};

// 与 NPC交互的任务 对话
USTRUCT()
struct PLANET_API FSTT_GuideInteractionConversation :
	public FSTT_GuideInteractionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideInteractionConversation;

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus EnterState(
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
struct PLANET_API FSTID_GuideInteractionOption :
	public FSTID_GuideInteractionTaskBase
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Interaction_Option> GameplayTaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
	
	UPROPERTY(EditAnywhere, Category = Param)
	TArray<FString> OptionAry;
	
	UPROPERTY(EditAnywhere, Category = Param)
	float DurationTime = -1.f;
	
};

// 与 NPC交互的任务 选项对话
USTRUCT()
struct PLANET_API FSTT_GuideInteractionOption :
	public FSTT_GuideInteractionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideInteractionOption;

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
	
	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

	EStateTreeRunStatus PerformMoveTask(FStateTreeExecutionContext& Context) const;
};

