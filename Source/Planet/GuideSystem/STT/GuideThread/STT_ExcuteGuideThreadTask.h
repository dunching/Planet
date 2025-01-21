// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateType.h"

#include "STT_ExcuteGuideThreadTask.generated.h"

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

USTRUCT()
struct PLANET_API FStateTreeExcuteGuideTaskTaskInstanceData
{
	GENERATED_BODY()

	FStateTreeExcuteGuideTaskTaskInstanceData();
	
	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AGuideThread> GuideActorPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Param)
	TSoftObjectPtr<UPAD_TaskNode_Guide> TaskNodeRef;

	UPROPERTY(VisibleAnywhere, Category = Param)
	FGuid TaskID;
	
	UPROPERTY(Transient)
	TObjectPtr<UGameplayTask_Base> GameplayTaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

	UPROPERTY(Transient)
	AHumanCharacter_Player* PlayerCharacterPtr = nullptr;

};

USTRUCT()
struct PLANET_API FSTT_ExcuteGuideThreadTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeExcuteGuideTaskTaskInstanceData;

	FSTT_ExcuteGuideThreadTask();

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
