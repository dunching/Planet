// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"

#include "GenerateTypes.h"

#include "STT_DashToLeader.generated.h"

class IGameplayTaskOwnerInterface;

class UAITask_DashToLeader;

class AHumanCharacter;
class AHumanAIController;

USTRUCT()
struct PLANET_API FStateTreeDashToLeaderTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter> CharacterPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanAIController> AIControllerPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Parameter)
	FVector Destination = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = Parameter)
	float AcceptableRadius = 200.f;

	UPROPERTY(Transient)
	TObjectPtr<UAITask_DashToLeader> AITaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
};

/*
 *	距离玩家过远时，或自动寻路跟随玩家失败时 强制冲刺至玩家身边
 */
USTRUCT()
struct PLANET_API FSTT_DashToLeader : public FStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeDashToLeaderTaskInstanceData;

	using FAITaskType = UAITask_DashToLeader;

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

	virtual FAITaskType* PerformGameplayTask(FStateTreeExecutionContext& Context) const;

};
