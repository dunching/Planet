// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"

#include "STT_QuestBase.h"

#include "STT_QuestInteractionBase.generated.h"

class AQuestInteractionBase;
class UGloabVariable_InteractionBase;
class AQuestChain_ImmediateBase;
class AQuestChain_BranchBase;

#pragma region Base
// 与 NPC交互的任务 基类
USTRUCT()
struct QUESTSYSTEM_API FSTID_GuideInteractionTaskBase :
	public FSTID_QuestBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AQuestInteractionBase> GuideActorPtr = nullptr;
};

USTRUCT()
struct QUESTSYSTEM_API FSTT_QuestInteractionBase :
	public FStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideInteractionTaskBase;

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
};
#pragma endregion

#pragma region 与 NPC交互的任务 提前终止
USTRUCT()
struct QUESTSYSTEM_API FSTT_GuideInteraction_Termination :
	public FSTT_QuestInteractionBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
};
#pragma endregion

#pragma region 与 NPC交互的任务 选择的内容通知到引导任务
USTRUCT()
struct QUESTSYSTEM_API FSTID_GuideInteractionNotify :
	public FSTID_GuideInteractionTaskBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	UGloabVariable_InteractionBase* GloabVariable = nullptr;
};

USTRUCT()
struct QUESTSYSTEM_API FSTT_GuideInteractionNotify :
	public FSTT_QuestInteractionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideInteractionNotify;

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
};
#pragma endregion

#pragma region 给玩家一条任务
USTRUCT()
struct QUESTSYSTEM_API FSTID_GuideInteraction_GiveGuideThread :
	public FSTID_GuideInteractionTaskBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Param)
	TSubclassOf<AQuestChain_ImmediateBase> ImmediateGuideThreadClass = nullptr;

	UPROPERTY(EditAnywhere, Category = Param)
	TSubclassOf<AQuestChain_BranchBase> BrandGuideThreadClass = nullptr;

	UPROPERTY(EditAnywhere, Category = Param)
	bool bIsAutomaticActive = true;
};

USTRUCT()
struct QUESTSYSTEM_API FSTT_GuideInteraction_GiveGuideThread :
	public FSTT_QuestInteractionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideInteraction_GiveGuideThread;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
};
#pragma endregion
