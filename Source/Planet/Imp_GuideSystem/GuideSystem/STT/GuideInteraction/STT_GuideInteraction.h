// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"

#include "STT_GuideInteractionBase.h"

#include "STT_GuideInteraction.generated.h"

class AGuideActor;
class AGuideThread;
class AGuideThread_Main;
class AGuideThread_Branch;
class AGuideThread_Immediate;
class AGuideInteractionBase;
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
struct PLANET_API FSTID_GuideInteractionTask :
	public FSTID_GuideInteractionTaskBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter_Player> PlayerCharacterPtr = nullptr;
};

USTRUCT()
struct PLANET_API FSTT_GuideInteraction :
	public FSTT_GuideInteractionBase
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
struct PLANET_API FSTT_GuideInteraction_BackToRegularProcessor :
	public FSTT_GuideInteraction
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
		) const override;
};
#pragma endregion
