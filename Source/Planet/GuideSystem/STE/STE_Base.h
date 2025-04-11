// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Templates/SubclassOf.h"
#include <Perception/AIPerceptionTypes.h>
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeExecutionContext.h"

#include "GenerateType.h"

#include "STE_Base.generated.h"

class AGuideActor;
class AGuideThread_Main;
class AGuideInteraction_Actor;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;

UCLASS(Blueprintable)
class PLANET_API USTE_Base : public UStateTreeEvaluatorBlueprintBase
{
	GENERATED_BODY()

public:

	virtual void TreeStop(FStateTreeExecutionContext& Context)override;

protected:

};
