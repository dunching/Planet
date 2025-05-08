// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HumanCharacter_AI.h"

#include "InputProcessor.h"

class AHumanCharacter_Player;
class AHumanCharacter_AI;

namespace HumanProcessor
{
	class FHumanInteractionBaseProcessor : public FInputProcessor
	{
	private:

		GENERATIONCLASSINFO(FHumanInteractionBaseProcessor, FInputProcessor);

	public:
		
		using FOwnerPawnType = APawn;

		FHumanInteractionBaseProcessor(FOwnerPawnType* CharacterPtr);

	};
}
