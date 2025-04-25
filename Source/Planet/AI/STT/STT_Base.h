// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "STT_Base.generated.h"

class AHumanAIController;
class AHumanCharacter;
class AHumanCharacter_AI;

USTRUCT()
struct PLANET_API FSTID_CharacterBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter_AI> CharacterPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanAIController> AIControllerPtr = nullptr;
};
