// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Templates/SubclassOf.h"
#include <Perception/AIPerceptionTypes.h>
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeExecutionContext.h"

#include "GenerateTypes.h"
#include "STE_CharacterBase.h"

#include "STE_Trader.generated.h"

class AActor;

struct FCharacterProxy;
class AGeneratorNPCs_Patrol;
class ACharacterBase;
class ABuildingArea;
class USceneComponent;
class AHumanCharacter;
class AHumanCharacter_AI;
class AHumanAIController;
class UGloabVariable_Character;

UCLASS(Blueprintable)
class PLANET_API UGloabVariable_TraderCharacter : public UGloabVariable_Character
{
	GENERATED_BODY()

public:
};

UCLASS(Blueprintable)
class PLANET_API USTE_Trader : public USTE_CharacterBase
{
	GENERATED_BODY()

public:
	using FGloabVariable = UGloabVariable_TraderCharacter;

protected:
	virtual void TreeStart(
		FStateTreeExecutionContext& Context
	) override;

private:
	virtual UGloabVariable_Character* CreateGloabVarianble() override;;

	TWeakObjectPtr<ACharacterBase> UpdateTargetCharacter();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Param)
	int32 MaxDistance = 200;
};
