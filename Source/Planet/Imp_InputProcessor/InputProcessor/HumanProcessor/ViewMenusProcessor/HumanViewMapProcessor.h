// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ViewMenuBaseProcessor.h"

class AHumanCharacter;

namespace HumanProcessor
{
	class PLANET_API FHumanViewMapProcessor : public FViewMenuBaseProcessor
	{
	private:

		GENERATIONCLASSINFO(FHumanViewMapProcessor, FViewMenuBaseProcessor);

	public:

		FHumanViewMapProcessor(FOwnerPawnType* CharacterPtr);

		virtual void EnterAction()override;

		virtual bool InputKey(
			const FInputKeyEventArgs& EventArgs
		) override;

		virtual void QuitAction()override;

	protected:

	private:

		void CheckInteraction();

		void QuitCurrentState();

	};
}
