// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"

#include "GenerateTypes.h"
#include "STT_CharacterBase.h"

#include "STT_UpdateBrandPromt.generated.h"

class IGameplayTaskOwnerInterface;

class UAITask_DashToLeader;

class AHumanCharacter;
class AHumanAIController;
class UTaskPromt;

USTRUCT()
struct PLANET_API FSTID_UpdateBrandPromt : public FSTID_CharacterBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	TSubclassOf<UTaskPromt> TaskPromtClass;

	UPROPERTY(EditAnywhere, Category = Output)
	bool bHaveTask = false;
};

/*
 * 是否有支线任务
 */
USTRUCT()
struct PLANET_API FSTT_UpdateBrandPromt : public FSTT_CharacterBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_UpdateBrandPromt;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
};
