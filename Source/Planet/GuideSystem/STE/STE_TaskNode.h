// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Templates/SubclassOf.h"
#include <Perception/AIPerceptionTypes.h>
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeExecutionContext.h"

#include "GenerateType.h"

#include "STE_TaskNode.generated.h"

class AGuideActor;
class AGuideMainThread;
class AGuideInteractionActor;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;

UCLASS(Blueprintable)
class PLANET_API USTE_TaskNode : public UStateTreeEvaluatorBlueprintBase
{
	GENERATED_BODY()

public:

protected:

};
