// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AISystem.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "Tasks/StateTreeAITask.h"
#include "Templates/SubclassOf.h"
#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"

#include "GenerateType.h"

#include "STT_MoveBySpline.generated.h"

class USplineComponent;
class IGameplayTaskOwnerInterface;
class AAIController;
class UAITask_MoveTo;
class UNavigationQueryFilter;
struct FAIMoveRequest;

class UAITask_MoveBySpline;
class AHumanCharacter;
class AHumanAIController;

USTRUCT()
struct PLANET_API FStateTreeMoveBySplineTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter> CharacterPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanAIController> AIControllerPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Parameter)
	USplineComponent* SPlinePtr = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAITask_MoveBySpline> AITaskPtr = nullptr;
	
	/** fixed distance added to threshold between AI and goal location in destination reach test */
	UPROPERTY(EditAnywhere, Category = Parameter, meta=(ClampMin = "0.0", UIMin="0.0"))
	float AcceptableRadius = GET_AI_CONFIG_VAR(AcceptanceRadius);

	/** "None" will result in default filter being used */
	UPROPERTY(EditAnywhere, Category = Parameter)
	TSubclassOf<UNavigationQueryFilter> FilterClass;

	UPROPERTY(EditAnywhere, Category = Parameter)
	bool bAllowStrafe = GET_AI_CONFIG_VAR(bAllowStrafing);

	/** if set, use incomplete path when goal can't be reached */
	UPROPERTY(EditAnywhere, Category = Parameter)
	bool bAllowPartialPath = GET_AI_CONFIG_VAR(bAcceptPartialPaths);

	/** if set, path to goal actor will update itself when actor moves */
	UPROPERTY(EditAnywhere, Category = Parameter)
	bool bTrackMovingGoal = true;

	/** if set, the goal location will need to be navigable */
	UPROPERTY(EditAnywhere, Category = Parameter)
	bool bRequireNavigableEndLocation = true;

	/** if set, goal location will be projected on navigation data (navmesh) before using */
	UPROPERTY(EditAnywhere, Category = Parameter)
	bool bProjectGoalLocation = true;

	/** if set, radius of AI's capsule will be added to threshold between AI and goal location in destination reach test  */
	UPROPERTY(EditAnywhere, Category = Parameter)
	bool bReachTestIncludesAgentRadius = GET_AI_CONFIG_VAR(bFinishMoveOnGoalOverlap);
	
	/** if set, radius of goal's capsule will be added to threshold between AI and goal location in destination reach test  */
	UPROPERTY(EditAnywhere, Category = Parameter)
	bool bReachTestIncludesGoalRadius = GET_AI_CONFIG_VAR(bFinishMoveOnGoalOverlap);

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

	UPROPERTY(Transient)
	int32 PointIndex = 0;

};

USTRUCT()
struct PLANET_API FSTT_MoveBySpline : public FStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeMoveBySplineTaskInstanceData;

	using FAITaskType = UAITask_MoveBySpline;

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

	virtual FAITaskType* PrepareMoveToTask(
		FStateTreeExecutionContext& Context,
		AAIController& Controller,
		FAITaskType* ExistingTask,
		FAIMoveRequest& MoveRequest
	) const;

	virtual EStateTreeRunStatus PerformMoveTask(
		FStateTreeExecutionContext& Context, 
		AAIController& Controller
	) const;

};