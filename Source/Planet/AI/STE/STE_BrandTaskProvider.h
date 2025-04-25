// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Templates/SubclassOf.h"
#include <Perception/AIPerceptionTypes.h>
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeExecutionContext.h"

#include "GenerateType.h"
#include "STE_CharacterBase.h"

#include "STE_BrandTaskProvider.generated.h"

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
class PLANET_API USTE_BrandTaskProvider : public USTE_CharacterBase
{
	GENERATED_BODY()

public:

protected:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Param)
	int32 MaxDistance = 200;

private:

	void CheckKnowCharacterImp();

	FTimerHandle CheckKnowCharacterTimerHandle;

};
