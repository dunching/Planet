// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ViewMenuBaseProcessor.h"

class AHumanCharacter;

namespace HumanProcessor
{
	class PLANET_INPUTPROCESSOR_API FViewBackpackProcessor : public FViewMenuBaseProcessor
	{
	private:

		GENERATIONCLASSINFO(FViewBackpackProcessor, FViewMenuBaseProcessor);

	public:

		FViewBackpackProcessor(FOwnerPawnType* CharacterPtr);

		virtual void EnterAction()override;

		virtual void QuitAction()override;

	protected:

	private:

		void CheckInteraction();

		void QuitCurrentState();

	};
}
