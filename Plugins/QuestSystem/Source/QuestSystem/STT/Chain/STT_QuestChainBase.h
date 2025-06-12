// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "STT_QuestBase.h"

#include "STT_QuestChainBase.generated.h"

class AQuestChainBase;

#pragma region Base
USTRUCT()
struct QUESTSYSTEM_API FSTID_QuestChainBase :
	public FSTID_QuestBase
{
	GENERATED_BODY()

	/**
	 * 该引导的的Actor
	 */
	UPROPERTY(
		EditAnywhere,
		Category = Context
	)
	TObjectPtr<AQuestChainBase> GuideThreadActorPtr = nullptr;
};

// 执行引导任务 
USTRUCT()
struct QUESTSYSTEM_API FSTT_QuestChainBase :
	public FStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_QuestChainBase;

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
	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const;
};
#pragma endregion

#pragma region Fail
USTRUCT()
struct QUESTSYSTEM_API FSTID_GuideThreadFail :
	public FSTID_QuestChainBase
{
	GENERATED_BODY()
};

// 执行引导任务 失败时
USTRUCT()
struct QUESTSYSTEM_API FSTT_GuideThreadFail :
	public FSTT_QuestChainBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
};
#pragma endregion
