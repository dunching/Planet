// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "Tasks/StateTreeMoveToTask.h"
#include "Tasks/StateTreeAITask.h"
#include "StateTreeExecutionContext.h"

#include "STT_MoveTo.generated.h"

USTRUCT(meta = (DisplayName = "MyMoveTo", Category = "AI|Action"))
struct PLANET_API FSTT_MoveToTask : public FStateTreeMoveToTask
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual void StateCompleted(
		FStateTreeExecutionContext& Context, 
		const EStateTreeRunStatus CompletionStatus, 
		const FStateTreeActiveStates& CompletedActiveStates
	) const override;

};
