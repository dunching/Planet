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

	bool GetPatrolPosition(float);

	FTSTicker::FDelegateHandle TickDelegateHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Context)
	AHumanCharacter* HumanCharacterPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Context)
	AHumanAIController* HumanAIControllerPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	ETeammateOption TeammateOption = ETeammateOption::kFollow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	AHumanCharacter* TargetCharacterPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	bool bIsFoundTarget = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	float GetPatrolPositionDelta = 5.0;

	TCallbackHandleContainer<void(ETeammateOption, AHumanCharacter*)>::FCallbackHandleSPtr TeammateOptionChangedDelegate;
	
	TCallbackHandleContainer<void()>::FCallbackHandleSPtr TeammateChangedDelegate;

};
