// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "STT_GuideBase.h"

#include "STT_GuideThreadBase.generated.h"

class AGuideThreadBase;

#pragma region Base
USTRUCT()
struct GUIDESYSTEM_API FSTID_GuideThreadBase :
	public FSTID_GuideBase
{
	GENERATED_BODY()

	/**
	 * 该引导的的Actor
	 */
	UPROPERTY(
		EditAnywhere,
		Category = Context
	)
	TObjectPtr<AGuideThreadBase> GuideThreadActorPtr = nullptr;
};

// 执行引导任务 
USTRUCT()
struct GUIDESYSTEM_API FSTT_GuideThreadBase :
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
	virtual FTaskNodeDescript GetTaskNodeDescripton(
		FStateTreeExecutionContext& Context
	) const;
};
#pragma endregion

#pragma region Fail
USTRUCT()
struct GUIDESYSTEM_API FSTID_GuideThreadFail :
	public FSTID_GuideThreadBase
{
	GENERATED_BODY()
};

// 执行引导任务 失败时
USTRUCT()
struct GUIDESYSTEM_API FSTT_GuideThreadFail :
	public FSTT_GuideThreadBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
};
#pragma endregion
