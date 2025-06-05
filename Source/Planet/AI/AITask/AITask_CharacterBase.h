// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "Tasks/AITask.h"

#include "AITask_CharacterBase.generated.h"

class AHumanCharacter_AI;
class AHumanAIController;

UCLASS()
class PLANET_API UAITask_CharacterBase : public UAITask
{
	GENERATED_BODY()

public:
	void SetUpController(
		AHumanCharacter_AI* PlayerCharacterPtr,
		AHumanAIController* ControllerPtr
	);
	;
	AHumanCharacter_AI* PlayerCharacterPtr = nullptr;

	AHumanAIController* ControllerPtr = nullptr;
};
