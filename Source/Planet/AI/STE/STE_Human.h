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

class UCharacterUnit;
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	bool bIsFarawayOriginal = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	TWeakObjectPtr<ACharacterBase> TargetCharacterPtr = nullptr;

};

UCLASS(Blueprintable)
class PLANET_API USTE_Human : public UStateTreeEvaluatorBlueprintBase
{
	GENERATED_BODY()

public:

	using FCharacterUnitType = UCharacterUnit;

	using FTeamOptionChangedHandle =
		TCallbackHandleContainer<void(ETeammateOption, FCharacterUnitType*)>::FCallbackHandleSPtr;

	using FTeammateChangedHandle =
		TCallbackHandleContainer<void()>::FCallbackHandleSPtr;

	using FKownCharacterChangedHandle =
		TCallbackHandleContainer<void(TWeakObjectPtr<ACharacterBase>, bool)>::FCallbackHandleSPtr;

protected:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	virtual void TreeStop(FStateTreeExecutionContext& Context)override;

	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime)override;

	void OnTeamOptionChanged(ETeammateOption TeammateOption);

	void OnTeamChanged();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	UGloabVariable* GloabVariable = nullptr;

public:

	void KnowCharaterChanged(TWeakObjectPtr<ACharacterBase> KnowCharacter, bool bIsAdd);

	FTSTicker::FDelegateHandle CaculationPatrolHandle;

	FTSTicker::FDelegateHandle CaculationDistance2AreaHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Context)
	ACharacterBase* HumanCharacterPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Context)
	AHumanAIController* HumanAIControllerPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Param)
	int32 MaxDistanceToPatrolSpline = 1000;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	ETeammateOption TeammateOption = ETeammateOption::kFollow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	ACharacterBase* LeaderCharacterPtr = nullptr;

	FTeamOptionChangedHandle TeammateOptionChangedDelegate;

	FTeammateChangedHandle TeammateChangedDelegate;

	FKownCharacterChangedHandle	KownCharacterChangedHandle;

};
