// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HumanCharacter_AI.h"

#include "InputProcessor.h"

class AHumanCharacter_Player;
class AHumanCharacter_AI;

namespace HumanProcessor
{
	class FHumanInteractionWithNPCProcessor : public FInputProcessor
	{
	private:

		GENERATIONCLASSINFO(FHumanInteractionWithNPCProcessor, FInputProcessor);

	public:

		using FOwnerPawnType = AHumanCharacter_Player;

		FHumanInteractionWithNPCProcessor(FOwnerPawnType* CharacterPtr);

		virtual void EnterAction()override;

		virtual void QuitAction() override;

		AHumanCharacter_AI* CharacterPtr = nullptr;
		
	protected:

		void Switch2RegularProcessor();

	};
}
