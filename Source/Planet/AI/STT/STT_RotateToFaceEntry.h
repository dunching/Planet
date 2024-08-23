// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateType.h"

#include "STT_RotateToFaceEntry.generated.h"

class UEnvQuery;

class ACharacterBase;
class AHumanCharacter;
class AHumanAIController;
class UAITask_ReleaseSkill;
class USTE_Human;
class UGloabVariable;

USTRUCT()
struct PLANET_API FStateTreeRotateToFaceEntryTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter> CharacterPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanAIController> AIControllerPtr = nullptr;
	
	UPROPERTY(EditAnywhere, Category = Context)
	TWeakObjectPtr<ACharacterBase> TargetCharacterPtr = nullptr;

};

USTRUCT()
struct PLANET_API FSTT_RotateToFaceEntry : public FStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeRotateToFaceEntryTaskInstanceData;

	using FAITaskType = UAITask_ReleaseSkill;

	FSTT_RotateToFaceEntry();

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

	virtual EStateTreeRunStatus PerformMoveTask(FStateTreeExecutionContext& Context)const;

};
