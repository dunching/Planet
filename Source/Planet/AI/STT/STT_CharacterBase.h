// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "Tasks/StateTreeAITask.h"

#include "STT_CharacterBase.generated.h"

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

/*
 */
USTRUCT()
struct PLANET_API FSTT_CharacterBase : public FStateTreeAIActionTaskBase //FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_CharacterBase;
};
