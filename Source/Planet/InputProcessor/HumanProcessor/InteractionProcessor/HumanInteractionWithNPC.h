// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "InputProcessor.h"

class AHumanCharacter_Player;

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

		virtual void ESCKeyPressed()override;

	protected:

		void Switch2RegularProcessor();

	};
}
