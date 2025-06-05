// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Templates/SubclassOf.h"
#include <Perception/AIPerceptionTypes.h>
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeExecutionContext.h"

#include "GenerateTypes.h"

#include "STE_CharacterBase.generated.h"

class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_AI;
class AHumanAIController;

UCLASS(Blueprintable)
class PLANET_API UGloabVariable_Character : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	FVector OriginalLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	FVector Location = FVector::ZeroVector;

	bool bEQSSucessed = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	int32 QueryDistance = 100;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	TWeakObjectPtr<ACharacterBase> TargetCharacterPtr = nullptr;

	std::function<TWeakObjectPtr<ACharacterBase>()> UpdateTargetCharacterFunc;
};

UCLASS(Blueprintable)
class PLANET_API USTE_CharacterBase : public UStateTreeEvaluatorBlueprintBase
{
	GENERATED_BODY()

protected:
	using FGloabVariable = UGloabVariable_Character;

	virtual void TreeStart(
		FStateTreeExecutionContext& Context
		) override;

	/**
	 * 选择最近的目标
	 * @param ChractersSet 
	 * @return 
	 */
	TWeakObjectPtr<ACharacterBase> GetNewTargetCharacter(
		const TSet<TWeakObjectPtr<ACharacterBase>>& ChractersSet
		) const;

private:
	virtual UGloabVariable_Character* CreateGloabVarianble();;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Context)
	AHumanCharacter_AI* HumanCharacterPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Context)
	AHumanAIController* HumanAIControllerPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	UGloabVariable_Character* GloabVariable_Character = nullptr;
};
