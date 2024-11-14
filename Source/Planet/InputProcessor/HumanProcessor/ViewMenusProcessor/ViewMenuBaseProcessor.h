// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "InputProcessor.h"

class AHumanCharacter_Player;

namespace HumanProcessor
{
	class FViewMenuBaseProcessor : public FInputProcessor
	{
	private:

		GENERATIONCLASSINFO(FViewMenuBaseProcessor, FInputProcessor);

	public:

		using FOwnerPawnType = AHumanCharacter_Player;

		FViewMenuBaseProcessor(FOwnerPawnType* CharacterPtr);

		virtual void ESCKeyPressed()override;

	protected:

		void Switch2RegularProcessor();

	};
}
