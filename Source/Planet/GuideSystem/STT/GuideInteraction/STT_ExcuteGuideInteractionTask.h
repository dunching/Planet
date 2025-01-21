// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateType.h"

#include "STT_ExcuteGuideInteractionTask.generated.h"

class AGuideActor;
class AGuideThread;
class AGuideMainThread;
class AGuideInteractionActor;
class UPAD_TaskNode_Guide;
class UPAD_TaskNode_Interaction;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;
class UGameplayTask_Base;

// 与 NPC交互的执行任务
USTRUCT()
struct PLANET_API FStateTreeExcuteGuideInteractionTaskInstanceData 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AGuideInteractionActor> GuideActorPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter_Player> PlayerCharacterPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter> TargetCharacterPtr = nullptr;

	// 上条任务的输出参数
	UPROPERTY(EditAnywhere, Category = Output)
	int32 LastTaskOut = 0;

	UPROPERTY(EditAnywhere, Category = Param)
	TSoftObjectPtr<UPAD_TaskNode_Interaction> TaskNodeRef;

	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Base> GameplayTaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

};

USTRUCT()
struct PLANET_API FSTT_ExcuteGuideInteractionTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeExcuteGuideInteractionTaskInstanceData;

	FSTT_ExcuteGuideInteractionTask();

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
