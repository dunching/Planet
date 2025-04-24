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

#include "STT_GuideInteraction.generated.h"

class AGuideActor;
class AGuideThread;
class AGuideThread_Main;
class AGuideThread_Branch;
class AGuideThread_Immediate;
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

#pragma region Base
// 与 NPC交互的任务 基类
USTRUCT()
struct PLANET_API FSTID_GuideInteractionTaskBase :
	public FSTID_GuideBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AGuideInteraction_Actor> GuideActorPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter_Player> PlayerCharacterPtr = nullptr;
};

USTRUCT()
struct PLANET_API FSTT_GuideInteractionBase :
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
struct PLANET_API FSTT_GuideInteraction_Termination :
	public FSTT_GuideInteractionBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
};
#pragma endregion

#pragma region 让玩家回到动作模式
USTRUCT()
struct PLANET_API FSTT_GuideInteraction_BackToRegularProcessor :
	public FSTT_GuideInteractionBase
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
struct PLANET_API FSTID_GuideInteractionNotify :
	public FSTID_GuideInteractionTaskBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	UGloabVariable_Interaction* GloabVariable = nullptr;
};

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
};
#pragma endregion

#pragma region 给玩家一条任务
USTRUCT()
struct PLANET_API FSTID_GuideInteraction_GiveGuideThread :
	public FSTID_GuideInteractionTaskBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Param)
	TSubclassOf<AGuideThread_Immediate> ImmediateGuideThreadClass = nullptr;

	UPROPERTY(EditAnywhere, Category = Param)
	TSubclassOf<AGuideThread_Branch> BrandGuideThreadClass = nullptr;

	UPROPERTY(EditAnywhere, Category = Param)
	bool bIsAutomaticActive = true;
};

USTRUCT()
struct PLANET_API FSTT_GuideInteraction_GiveGuideThread :
	public FSTT_GuideInteractionBase
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
