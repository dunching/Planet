// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "InputProcessor.h"

class AHumanCharacter_Player;

namespace HumanProcessor
{
	class PLANET_API FViewMenuBaseProcessor : public FInputProcessor
	{
	private:

		GENERATIONCLASSINFO(FViewMenuBaseProcessor, FInputProcessor);

	public:

		using FOwnerPawnType = AHumanCharacter_Player;

		FViewMenuBaseProcessor(FOwnerPawnType* CharacterPtr);

	protected:

		virtual void EnterAction()override;

		virtual bool InputKey(
			const FInputKeyEventArgs& EventArgs
		) override;

		void Switch2RegularProcessor();

	};
}
