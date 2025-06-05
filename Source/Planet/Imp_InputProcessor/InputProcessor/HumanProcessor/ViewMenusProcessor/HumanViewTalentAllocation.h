// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ViewMenuBaseProcessor.h"

class AHumanCharacter;

namespace HumanProcessor
{
	class PLANET_API FHumanViewTalentAllocation : public FViewMenuBaseProcessor
	{
	private:

		GENERATIONCLASSINFO(FHumanViewTalentAllocation, FViewMenuBaseProcessor);

	public:

		FHumanViewTalentAllocation(FOwnerPawnType* CharacterPtr);

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
