// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateTypes.h"
#include "STT_QuestBase.h"
#include "STT_CommonData.h"
#include "QuestsActorBase.h"
#include "STT_QuestInteraction.h"

#include "STT_GuideInteraction_HumanNPC.generated.h"

class AGuideActor;
class AQuestChain;
class AQuestChain_Main;
class AQuestInteractionBase;
class UPAD_TaskNode_Guide;
class UPAD_TaskNode_Interaction;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_AI;
class AHumanCharacter_Player;
class UGloabVariable_Interaction;
class UGameplayTask_Quest;
class UGameplayTask_Interaction_Conversation;
class UGameplayTask_Interaction_Option;
class UGameplayTask_Interaction_NotifyGuideThread;
class UGameplayTask_Interaction_Transaction;

#pragma region 与 NPC交互的任务 选项对话
USTRUCT()
struct PLANET_API FSTID_GuideInteractionOption :
	public FSTID_GuideInteractionTask
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Interaction_Option> GameplayTaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	UGloabVariable_InteractionBase* GloabVariable = nullptr;
	
	UPROPERTY(EditAnywhere, Category = Param)
	TArray<FString> OptionAry;

	UPROPERTY(EditAnywhere, Category = Param)
	float DurationTime = -1.f;
};

USTRUCT()
struct PLANET_API FSTT_GuideInteractionOption :
	public FSTT_QuestInteractionBase
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
	public FSTID_GuideInteractionTask
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Interaction_Conversation> GameplayTaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter_AI> TargetCharacterPtr = nullptr;
	
	UPROPERTY(EditAnywhere, Category = Param)
	TArray<FTaskNode_Conversation_SentenceInfo> ConversationsAry;
	
	/**
	 * 是否仅在人物上方的气泡上显示，而非弹出对话框
	 */
	UPROPERTY(EditAnywhere, Category = Param)
	bool bOnlyDisplyOnTitle = false;
};

// 与 NPC交互的任务 对话
USTRUCT()
struct PLANET_API FSTT_GuideInteractionConversation :
	public FSTT_QuestInteractionBase
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

#pragma region 改变指定NPC的可交互选项的是否可用状态
UCLASS(
	Blueprintable,
	BlueprintType
)
class PLANET_API UPAD_GuideInteraction_ChangeNPCsInteractionList : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(
		BlueprintReadOnly,
		EditAnywhere
	)
	TSoftObjectPtr<AHumanCharacter_AI> TargetCharacterPtr = nullptr;
};

USTRUCT()
struct PLANET_API FSTID_GuideInteraction_ChangeNPCsTaskState :
	public FSTID_GuideInteractionTask
{
	GENERATED_BODY()

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	TSoftObjectPtr<UPAD_GuideInteraction_ChangeNPCsInteractionList> PAD = nullptr;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	FGuid CharacterID;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	TSubclassOf<AQuestInteractionBase> GuideInteractionActorClass;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	bool bEnable = false;

	/**
	 * true,任务立即完成
	 * false,在子任务持续期间内保持状态
	 */
	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	bool bRunForever = false;
};

USTRUCT()
struct PLANET_API FSTT_GuideInteraction_ChangeNPCsTaskState :
	public FSTT_QuestInteractionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideInteraction_ChangeNPCsTaskState;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

protected:
};
#pragma endregion

#pragma region 与商人进行交易，打开交易界面
USTRUCT()
struct PLANET_API FSTID_GuideInteraction_Transaction :
	public FSTID_GuideInteractionTask
{
	GENERATED_BODY()
	
	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Interaction_Transaction> GameplayTaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter_AI> TargetCharacterPtr = nullptr;
	
};

USTRUCT()
struct PLANET_API FSTT_GuideInteraction_Transaction :
	public FSTT_QuestInteractionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideInteraction_Transaction;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

protected:
};
#pragma endregion
