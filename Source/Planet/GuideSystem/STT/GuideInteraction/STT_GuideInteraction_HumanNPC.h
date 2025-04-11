// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateType.h"
#include "STT_GuideBase.h"
#include "STT_CommonData.h"
#include "GuideActor.h"
#include "STT_GuideInteraction.h"

#include "STT_GuideInteraction_HumanNPC.generated.h"

class AGuideActor;
class AGuideThread;
class AGuideThread_Main;
class AGuideInteraction_Actor;
class UPAD_TaskNode_Guide;
class UPAD_TaskNode_Interaction;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;
class UGloabVariable_Interaction;
class UGameplayTask_Base;
class UGameplayTask_Interaction_Conversation;
class UGameplayTask_Interaction_Option;
class UGameplayTask_Interaction_NotifyGuideThread;

#pragma region 与 NPC交互的任务 选项对话
USTRUCT()
struct PLANET_API FSTID_GuideInteractionOption :
	public FSTID_GuideInteractionTaskBase
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Interaction_Option> GameplayTaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	UGloabVariable_Interaction* GloabVariable = nullptr;
	
	UPROPERTY(EditAnywhere, Category = Param)
	TArray<FString> OptionAry;

	UPROPERTY(EditAnywhere, Category = Param)
	float DurationTime = -1.f;
};

USTRUCT()
struct PLANET_API FSTT_GuideInteractionOption :
	public FSTT_GuideInteractionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideInteractionOption;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	};

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

	EStateTreeRunStatus PerformGameplayTask(FStateTreeExecutionContext& Context) const;
};
#pragma endregion

#pragma region 与NPC对话
USTRUCT()
struct PLANET_API FSTID_GuideInteractionConversation :
	public FSTID_GuideInteractionTaskBase
{
	GENERATED_BODY()


	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Interaction_Conversation> GameplayTaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter> TargetCharacterPtr = nullptr;
	
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

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	};

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

	EStateTreeRunStatus PerformGameplayTask(FStateTreeExecutionContext& Context) const;
};
#pragma endregion
