// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "Tasks\StateTreeMoveToTask.h"
#include "StateTreeExecutionContext.h"

#include "STT_MoveTo.generated.h"

USTRUCT(meta = (DisplayName = "MyMoveTo", Category = "AI|Action"))
struct PLANET_API FSTT_MoveToTask : public FStateTreeMoveToTask
{
	GENERATED_BODY()

	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

};
