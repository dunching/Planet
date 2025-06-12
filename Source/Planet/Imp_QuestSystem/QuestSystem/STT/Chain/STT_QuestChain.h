// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"

#include "LayoutCommon.h"
#include "MainMenuCommon.h"
#include "STT_QuestBase.h"
#include "STT_CommonData.h"
#include "STT_QuestChainBase.h"
#include "TeamMates_GenericType.h"


#include "STT_QuestChain.generated.h"

class UGameplayAbility;
class UPlanetGameplayAbility;
class UMarkPoints;
class ASceneActor;
class AResourceBoxBase;
class AChallengeEntry;
class ATargetPoint_Runtime;
class AGeneratorColony_ByTime;
class ATargetPoint;
class AGeneratorBase;
class AAreaVolume;
class AGuideActor;
class AQuestChain;
class AQuestChain_Main;
class AQuestInteractionBase;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_AI;
class AHumanCharacter_Player;
class UPAD_TaskNode_Guide;
class UPAD_TaskNode_Interaction;
class UPAD_TaskNode_Guide_LeaveHere;
class UPAD_TaskNode_Guide_SpwanNPCColony;
class UGloabVariable_GuideBrandThreadBase;
class UGloabVariable_GuideThread_AreaBase;
class UGloabVariable_GuideThread_MainBase;
class UGameplayTask_Quest;
class UGameplayTask_Guide_WaitComplete;
class UGameplayTask_Guide_ConversationWithTarget;
class UGameplayTask_Guide_AddToTarget;
class UGameplayTask_Guide_CollectResource;
class UGameplayTask_Guide_DefeatEnemy;
class UGameplayTask_Guide_ReturnOpenWorld;
class UGameplayTask_Guide_ActiveDash;
class UGameplayTask_Guide_ActiveRun;
class UGameplayTask_Guide_AttckCharacter;
class UGameplayTask_WaitInteractionSceneActor;
class UGameplayTask_WaitPlayerEquipment;
class UGameplayTask_WaitOpenLayout;

struct FConsumableProxy;
struct FTaskNode_Conversation_SentenceInfo;

#pragma region Base
USTRUCT()
struct PLANET_API FSTID_GuideThread :
	public FSTID_QuestChainBase
{
	GENERATED_BODY()

	/**
	 * 该引导的的PlayerCharacter
	 */
	UPROPERTY(
		Transient
	)
	AHumanCharacter_Player* PlayerCharacterPtr = nullptr;
};

// 执行引导任务 
USTRUCT()
struct PLANET_API FSTT_QuestChain :
	public FSTT_QuestChainBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThread;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

protected:
	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;

};
#pragma endregion

#pragma region 要求玩家按下某个按键
USTRUCT()
struct PLANET_API FSTID_GuideThread_PressKey :
	public FSTID_GuideThread
{
	GENERATED_BODY()

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	FKey Key = EKeys::AnyKey;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	FString Description = TEXT("Decription {Key} To Replace Str");

	APlayerController* PCPtr = nullptr;
};

// 
USTRUCT()
struct PLANET_API FSTT_QuestChain_PressKey :
	public FSTT_QuestChain
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThread_PressKey;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;
};
#pragma endregion

#pragma region 要求玩家切换到某个UI布局
USTRUCT()
struct PLANET_API FSTID_GuideThread_OpenLayout :
	public FSTID_GuideThread
{
	GENERATED_BODY()

	UPROPERTY(
		Transient
	)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

	UPROPERTY(
		Transient
	)
	TObjectPtr<UGameplayTask_WaitOpenLayout> GameplayTaskPtr = nullptr;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	ELayoutCommon LayoutCommon = ELayoutCommon::kMenuLayout;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	EMenuType MenuType = EMenuType::kAllocationSkill;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	FString Description = TEXT("Decription {Layout} &{MenuLayout} To Replace Str");
};

// 
USTRUCT()
struct PLANET_API FSTT_GuideThread_OpenLayout :
	public FSTT_QuestChain
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThread_OpenLayout;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;
};
#pragma endregion

#pragma region 要求玩家激活一次冲刺
USTRUCT()
struct PLANET_API FSTID_GuideThread_ActiveDash :
	public FSTID_GuideThread
{
	GENERATED_BODY()

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	TSubclassOf<UPlanetGameplayAbility> GAClass;

	UPROPERTY(
		Transient
	)
	TObjectPtr<UGameplayTask_Guide_ActiveDash> GameplayTaskPtr = nullptr;

	UPROPERTY(
		Transient
	)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
};

// 
USTRUCT()
struct PLANET_API FSTT_GuideThread_ActiveDash :
	public FSTT_QuestChain
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThread_ActiveDash;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

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

	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;

private:
};
#pragma endregion

#pragma region 要求玩家激活奔跑
USTRUCT()
struct PLANET_API FSTID_GuideThread_Run :
	public FSTID_GuideThread
{
	GENERATED_BODY()

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	TSubclassOf<UPlanetGameplayAbility> GAClass;

	UPROPERTY(
		Transient
	)
	TObjectPtr<UGameplayTask_Guide_ActiveRun> GameplayTaskPtr = nullptr;

	UPROPERTY(
		Transient
	)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
};

// 
USTRUCT()
struct PLANET_API FSTT_GuideThread_Run :
	public FSTT_QuestChain
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThread_Run;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

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

	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;

private:
};
#pragma endregion

#pragma region 要求玩家弹出光标
USTRUCT()
struct PLANET_API FSTID_GuideThread_ShowCursor :
	public FSTID_GuideThread
{
	GENERATED_BODY()

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	FString Description = TEXT("Hold down the <RichText.Key>{Key} key</> to display the cursor");

	FKey Key = EKeys::AnyKey;

	APlayerController* PCPtr = nullptr;
};

// 
USTRUCT()
struct PLANET_API FSTT_GuideThread_ShowCursor :
	public FSTT_QuestChain
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThread_ShowCursor;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;

private:
};
#pragma endregion

#pragma region 引导玩家去往指定地点
UCLASS(
	Blueprintable,
	BlueprintType
)
class PLANET_API UPAD_GuideThread_GoToTheTargetPoint : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(
		BlueprintReadOnly,
		EditAnywhere
	)
	TSoftObjectPtr<ATargetPoint> TargetPointPtr = nullptr;

	UPROPERTY(
		BlueprintReadOnly,
		EditAnywhere
	)
	TSoftObjectPtr<AHumanCharacter_AI> TargetCharacterPtr = nullptr;
};

USTRUCT()
struct PLANET_API FSTID_GuideThread_GoToTheTargetPoint :
	public FSTID_GuideThread
{
	GENERATED_BODY()

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	TSoftObjectPtr<UPAD_GuideThread_GoToTheTargetPoint> PAD = nullptr;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	int32 ReachedRadius = 200;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	FString PromtStr;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	TSubclassOf<ATargetPoint_Runtime>TargetPoint_RuntimeClass;

	FVector TargetLocation = FVector::ZeroVector;

	TObjectPtr<AHumanCharacter_AI> TargetCharacterPtr = nullptr;

	ATargetPoint_Runtime* TargetPointPtr = nullptr;

	UMarkPoints* MarkPointsPtr = nullptr;
};

// 
USTRUCT()
struct PLANET_API FSTT_GuideThread_GoToTheTargetPoint :
	public FSTT_QuestChain
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThread_GoToTheTargetPoint;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

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

	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;
};
#pragma endregion

#pragma region 给玩家奖励物品
USTRUCT()
struct PLANET_API FSTID_GuideThreadDistributeRewards :
	public FSTID_GuideThread
{
	GENERATED_BODY()

	/**
	 * 任务完成后的奖励
	 */
	UPROPERTY(
		EditAnywhere,
		Category = Param
		)
	TMap<FGameplayTag, int32> RewardProxysMap;

};

// 执行任务 获取奖励
USTRUCT()
struct PLANET_API FSTT_GuideThreadDistributeRewards :
	public FSTT_QuestChain
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

#pragma region 独白
USTRUCT()
struct PLANET_API FSTID_GuideThreadMonologue :
	public FSTID_GuideThread
{
	GENERATED_BODY()

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	TArray<FTaskNode_Conversation_SentenceInfo> ConversationsAry;

	UPROPERTY(
		Transient
	)
	TObjectPtr<UGameplayTask_Quest> GameplayTaskPtr = nullptr;

	UPROPERTY(
		Transient
	)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
};

// 执行引导任务 对话
USTRUCT()
struct PLANET_API FSTT_GuideThreadMonologue :
	public FSTT_QuestChain
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

	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

protected:
	EStateTreeRunStatus PerformGameplayTask(
		FStateTreeExecutionContext& Context
	) const;

	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;

	float RemainingTime = 0.f;

	int32 SentenceIndex = 0;
};
#pragma endregion

#pragma region 等待其他引导通知
USTRUCT()
struct PLANET_API FSTID_GuideThread_WaitComplete :
	public FSTID_GuideThread
{
	GENERATED_BODY()

	// 通知的任务ID
	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	FGuid NotifyTaskID;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	FString PromtStr;
};

// 等待完成
USTRUCT()
struct PLANET_API FSTT_GuideThread_WaitTaskComplete :
	public FSTT_QuestChain
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThread_WaitComplete;

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
	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;
};
#pragma endregion

#pragma region 要求玩家与指定资源或NPC交互
UCLASS(
	Blueprintable,
	BlueprintType
)
class PLANET_API UPAD_GuideThread_WaitInteractionSceneActor : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(
		BlueprintReadOnly,
		EditAnywhere
	)
	TSoftObjectPtr<AHumanCharacter> CharacterPtr = nullptr;

	UPROPERTY(
		BlueprintReadOnly,
		EditAnywhere
	)
	TSoftObjectPtr<AResourceBoxBase> ResourceBoxPtr = nullptr;

	UPROPERTY(
		BlueprintReadOnly,
		EditAnywhere
	)
	TSoftObjectPtr<AChallengeEntry> ChallengeEntryPtr = nullptr;

	UPROPERTY(
		BlueprintReadOnly,
		EditAnywhere
	)
	TSubclassOf<AResourceBoxBase> ResourceBoxClass = nullptr;
};

USTRUCT()
struct PLANET_API FSTID_GuideThread_WaitInteractionSceneActor :
	public FSTID_GuideThread
{
	GENERATED_BODY()

	UPROPERTY(
		Transient
	)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

	UPROPERTY(
		Transient
	)
	TObjectPtr<UGameplayTask_WaitInteractionSceneActor> GameplayTaskPtr = nullptr;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	TSoftObjectPtr<UPAD_GuideThread_WaitInteractionSceneActor> PAD = nullptr;
	
	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	TSubclassOf<ATargetPoint_Runtime>TargetPoint_RuntimeClass;

};

// 等待完成
USTRUCT()
struct PLANET_API FSTT_GuideThread_WaitInteractionSceneActor :
	public FSTT_QuestChain
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThread_WaitInteractionSceneActor;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

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

protected:
	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;
};

#pragma endregion

#pragma region 要求玩家采集某种资源
USTRUCT()
struct PLANET_API FSTID_GuideThreadCollectResource :
	public FSTID_GuideThread
{
	GENERATED_BODY()

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	FGameplayTag ResourceType;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	int32 Num = 1;

	UPROPERTY(
		Transient
	)
	TObjectPtr<UGameplayTask_Guide_CollectResource> GameplayTaskPtr = nullptr;

	UPROPERTY(
		Transient
	)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
};

// 要求玩家采集指定类型的资源
USTRUCT()
struct PLANET_API FSTT_GuideThreadCollectResource :
	public FSTT_QuestChain
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

	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

protected:
	EStateTreeRunStatus PerformGameplayTask(
		FStateTreeExecutionContext& Context
	) const;

	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;
};
#pragma endregion

#pragma region 要求玩家击败某种类型的NPC
USTRUCT()
struct PLANET_API FSTID_GuideThreadDefeatEnemy :
	public FSTID_GuideThread
{
	GENERATED_BODY()

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	FGameplayTag EnemyType;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	int32 Num = 1;

	UPROPERTY(
		Transient
	)
	TObjectPtr<UGameplayTask_Guide_DefeatEnemy> GameplayTaskPtr = nullptr;

	UPROPERTY(
		Transient
	)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
};

// 要求玩家击败指定类型的敌人
USTRUCT()
struct PLANET_API FSTT_GuideThreadDefeatEnemy :
	public FSTT_QuestChain
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

	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

protected:
	EStateTreeRunStatus PerformGameplayTask(
		FStateTreeExecutionContext& Context
	) const;

	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;
};
#pragma endregion

#pragma region 改变指定NPC的可交互选项的是否可用状态
UCLASS(
	Blueprintable,
	BlueprintType
)
class PLANET_API UPAD_GuideThread_ChangeNPCsInteractionList : public UDataAsset
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
struct PLANET_API FSTID_GuideThread_ChangeNPCsInteractionList :
	public FSTID_GuideThread
{
	GENERATED_BODY()

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	TSoftObjectPtr<UPAD_GuideThread_ChangeNPCsInteractionList> PAD = nullptr;

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
	bool bEnable = true;

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

// 执行引导任务 给目标角色添加互动引导内容
USTRUCT()
struct PLANET_API FSTT_GuideThread_ChangeNPCsInteractionList :
	public FSTT_QuestChain
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThread_ChangeNPCsInteractionList;

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

#pragma region 要求玩家击败离开某个区域
UCLASS(
	Blueprintable,
	BlueprintType
)
class PLANET_API UPAD_GuideThread_LeaveHere : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(
		BlueprintReadOnly,
		EditAnywhere
	)
	TSoftObjectPtr<AAreaVolume> AreaVolumePtr = nullptr;
};

USTRUCT()
struct PLANET_API FSTID_GuideThreadLeaveHere :
	public FSTID_GuideThread
{
	GENERATED_BODY()

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	FTaskNode_Conversation_SentenceInfo Prompt;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	TSoftObjectPtr<UPAD_GuideThread_LeaveHere> PAD;
};

// 执行引导任务 给目标角色添加互动引导内容
USTRUCT()
struct PLANET_API FSTT_GuideThreadLeaveHere :
	public FSTT_QuestChain
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThreadLeaveHere;

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;

protected:
	float TatolTime = 0.f;
};
#pragma endregion

#pragma region 确认玩家是否在指定区域内
UCLASS(
	Blueprintable,
	BlueprintType
)
class PLANET_API UPAD_GuideThread_CheckIsInValidArea : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(
		BlueprintReadOnly,
		EditAnywhere
	)
	TSoftObjectPtr<AAreaVolume> AreaVolumePtr = nullptr;
};

USTRUCT()
struct PLANET_API FSTID_GuideThreadCheckIsInValidArea :
	public FSTID_GuideThread
{
	GENERATED_BODY()

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	TSoftObjectPtr<UPAD_GuideThread_CheckIsInValidArea> PAD;

	float Total = 0.f;
};

/**
 * 执行全局引导任务 确认玩家是否在有效区域内，若离开区域，则结束引导
 */
USTRUCT()
struct PLANET_API FSTGT_GuideThreadCheckIsInValidArea :
	public FSTT_QuestChain
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThreadCheckIsInValidArea;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;

protected:
};
#pragma endregion

#pragma region 返回开放世界
USTRUCT()
struct PLANET_API FSTID_GuideThreadReturnOpenWorld :
	public FSTID_GuideThread
{
	GENERATED_BODY()

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	FTaskNode_Conversation_SentenceInfo Prompt;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	int32 RemainTime = 5;

	UPROPERTY(
		Transient
	)
	TObjectPtr<UGameplayTask_Guide_ReturnOpenWorld> GameplayTaskPtr = nullptr;

	UPROPERTY(
		Transient
	)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
};

// 执行引导任务 给目标角色添加互动引导内容
USTRUCT()
struct PLANET_API FSTT_GuideThreadReturnOpenWorld :
	public FSTT_QuestChain
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThreadReturnOpenWorld;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

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

	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;

protected:
};
#pragma endregion

#pragma region 等待玩家在指定插槽内装备某中物品
USTRUCT()
struct PLANET_API FSTID_GuideThread_WaitPlayerEquipment :
	public FSTID_GuideThread
{
	GENERATED_BODY()

	UPROPERTY(
		Transient
	)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

	UPROPERTY(
		Transient
	)
	TObjectPtr<UGameplayTask_WaitPlayerEquipment> GameplayTaskPtr = nullptr;

	/**
	 * true 给玩家装备
	 * false 给NPC队友装备
	 */
	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	bool bPlayerAssign = true;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	FGameplayTag WeaponSocket;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	FGameplayTag SkillSocket;

	/**
	 * 是否是装备一个NPC队友
	 */
	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	bool bIsEquipentCharacter = false;
};

// 执行引导任务 给目标角色添加互动引导内容
USTRUCT()
struct PLANET_API FSTT_GuideThread_WaitPlayerEquipment :
	public FSTT_QuestChain
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThread_WaitPlayerEquipment;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

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

	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;

protected:
};
#pragma endregion

#pragma region 生成一个目标NPC
USTRUCT()
struct PLANET_API FSTID_GuideThread_SpawnNPC :
	public FSTID_GuideThread
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Context)
	UGloabVariable_GuideThread_MainBase* GloabVariable_Main = nullptr;
	
	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	TSubclassOf<AHumanCharacter_AI> NPCClass;

	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	ETeammateOption TeammateOption = ETeammateOption::kEnemy;
	
	FGuid NPC_ID;
};

// 执行引导任务 给目标角色添加互动引导内容
USTRUCT()
struct PLANET_API FSTT_GuideThread_SpawnNPC :
	public FSTT_QuestChain
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThread_SpawnNPC;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;

protected:
};
#pragma endregion

#pragma region 要求玩家攻击这个目标
USTRUCT()
struct PLANET_API FSTID_GuideThread_AttckCharacter :
	public FSTID_GuideThread
{
	GENERATED_BODY()
	
	UPROPERTY(
		Transient
	)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

	UPROPERTY(
		Transient
	)
	TObjectPtr<UGameplayTask_Guide_AttckCharacter> GameplayTaskPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	UGloabVariable_GuideThread_MainBase* GloabVariable_Main = nullptr;

	/**
	 * 击杀目标或仅攻击一次
	 */
	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	bool bIsKill = true;

	TObjectPtr<ACharacterBase> HumanCharacterAI = nullptr;

	bool bIsNotFreshed = true;
};

// 执行引导任务 给目标角色添加互动引导内容
USTRUCT()
struct PLANET_API FSTT_GuideThread_AttckCharacter :
	public FSTT_QuestChain
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThread_AttckCharacter;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

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

	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;

protected:
};
#pragma endregion

#pragma region 未完待续
USTRUCT()
struct PLANET_API FSTID_GuideThread_ToBeContinued :
	public FSTID_GuideThread
{
	GENERATED_BODY()
	
};

// 执行引导任务 给目标角色添加互动引导内容
USTRUCT()
struct PLANET_API FSTT_GuideThread_ToBeContinued :
	public FSTT_QuestChain
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThread_ToBeContinued;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;

protected:
};
#pragma endregion

#pragma region 引导完成
USTRUCT()
struct PLANET_API FSTID_GuideThread_Completet :
	public FSTID_GuideThread
{
	GENERATED_BODY()
	
};

USTRUCT()
struct PLANET_API FSTT_GuideThread_Completet :
	public FSTT_QuestChain
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideThread_Completet;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

protected:
};
#pragma endregion

#pragma region 延迟任务
USTRUCT()
struct PLANET_API FSTID_DelayTask :
	public FSTID_GuideThread
{
	GENERATED_BODY()

	/**
	 * 延迟时间
	 */
	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	int32 DelayTime = 5;

	/**
	 * 描述文字
	 */
	UPROPERTY(
		EditAnywhere,
		Category = Param
	)
	FString Descrption = TEXT("剩余[Time]秒");

	float RemainTime = 0.f;
};

/**
 * 与FStateTreeDelayTask的区别为输出一段描述文字
 */
USTRUCT()
struct PLANET_API FSTT_DelayTask :
	public FSTT_QuestChain
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_DelayTask;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const override;

protected:
};
#pragma endregion
