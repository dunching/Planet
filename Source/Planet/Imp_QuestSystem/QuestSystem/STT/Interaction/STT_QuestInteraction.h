// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"

#include "STT_QuestInteractionBase.h"

#include "STT_QuestInteraction.generated.h"

class AGuideActor;
class AQuestChain;
class AQuestChain_Main;
class AQuestChain_Branch;
class AQuestChain_Immediate;
class AQuestInteractionBase;
class UPAD_TaskNode_Guide;
class UPAD_TaskNode_Interaction;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;
class UGloabVariable_Interaction;
class UGameplayTask_Quest;
class UGameplayTask_Interaction_Conversation;
class UGameplayTask_Interaction_Option;
class UGameplayTask_Interaction_NotifyGuideThread;

#pragma region Base
// 与 NPC交互的任务 基类
USTRUCT()
struct PLANET_API FSTID_GuideInteractionTask :
	public FSTID_GuideInteractionTaskBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter_Player> PlayerCharacterPtr = nullptr;
};

USTRUCT()
struct PLANET_API FSTT_QuestInteraction :
	public FSTT_QuestInteractionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideInteractionTask;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
};
#pragma endregion

#pragma region 让玩家回到动作模式
USTRUCT()
struct PLANET_API FSTT_QuestInteraction_BackToRegularProcessor :
	public FSTT_QuestInteraction
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
		) const override;
};
#pragma endregion
