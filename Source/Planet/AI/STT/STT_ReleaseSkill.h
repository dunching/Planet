// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include "AISystem.h"

#include "GenerateType.h"

#include "STT_ReleaseSkill.generated.h"

class AHumanCharacter;
class AHumanAIController;
class UAITask_ReleaseSkill;
class UGloabVariable_Character;

USTRUCT()
struct PLANET_API FStateTreeReleaseSkillTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter> CharacterPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanAIController> AIControllerPtr = nullptr;
	
	UPROPERTY(EditAnywhere, Category = Context)
	UGloabVariable_Character* GloabVariable = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAITask_ReleaseSkill> AITaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
};

USTRUCT()
struct PLANET_API FSTT_ReleaseSkill : public FStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeReleaseSkillTaskInstanceData;

	using FAITaskType = UAITask_ReleaseSkill;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

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

	virtual EStateTreeRunStatus PerformGameplayTask(FStateTreeExecutionContext& Context) const;

};