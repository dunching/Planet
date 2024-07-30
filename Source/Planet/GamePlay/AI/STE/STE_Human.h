// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Templates/SubclassOf.h"
#include <Perception/AIPerceptionTypes.h>
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "StateTreeExecutionContext.h"

#include "GenerateType.h"

#include "STE_Human.generated.h"

class AActor;

class ACharacterBase;
class AHumanCharacter;
class AHumanAIController;


UCLASS(Blueprintable)
class PLANET_API UGloabVariable : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	FVector Location = FVector::ZeroVector;

};

UCLASS(Blueprintable)
class PLANET_API USTE_Human : public UStateTreeEvaluatorBlueprintBase
{
	GENERATED_BODY()

public:

protected:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	virtual void TreeStop(FStateTreeExecutionContext& Context)override;

	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime)override;

	void OnTeamOptionChanged(ETeammateOption TeammateOption);

	void OnTeamChanged();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	UGloabVariable* GloabVariable = nullptr;

public:

	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	bool UpdateInArea(float DletaTime);
	
	void CaculationPatrolPosition();

	bool GetPatrolPosition(float);

	FTSTicker::FDelegateHandle CaculationPatrolHandle;

	FTSTicker::FDelegateHandle CaculationDistance2AreaHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Context)
	ACharacterBase* HumanCharacterPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Context)
	AHumanAIController* HumanAIControllerPtr = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Context)
	int32 MaxDistanceToPatrolSpline = 1000;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	ETeammateOption TeammateOption = ETeammateOption::kFollow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	ACharacterBase* LeaderCharacterPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	ACharacterBase* TargetCharacterPtr = nullptr;

	TSet<ACharacterBase*>TargetSet;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	float GetPatrolPositionDelta = 5.0;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = Output)
	bool bIsInArea = true;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = Output)
	bool bIsFarwayPatrolSpline= true;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = Output)
	bool bIsNeedRun = false;

	TCallbackHandleContainer<void(ETeammateOption, ACharacterBase*)>::FCallbackHandleSPtr TeammateOptionChangedDelegate;
	
	TCallbackHandleContainer<void()>::FCallbackHandleSPtr TeammateChangedDelegate;

	FTimerHandle RemoveTarget;

};
