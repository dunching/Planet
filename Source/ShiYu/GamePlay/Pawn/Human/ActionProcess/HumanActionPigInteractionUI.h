// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "HumanProcessor.h"

class AHumanCharacter;

namespace HumanProcessor
{
	class FHumanActionPigInteractionUI : public FHumanProcessor
	{

	private:

		GENERATIONCLASSINFO(FHumanActionPigInteractionUI, FHumanProcessor);

	public:

		FHumanActionPigInteractionUI(AHumanCharacter* CharacterPtr);

		virtual ~FHumanActionPigInteractionUI();

		virtual void EnterAction()override;

		virtual void QuitAction()override;

		virtual void BKeyPressed()override;

		virtual void EKeyPressed()override;

		virtual void ESCKeyPressed()override;

	private:

		void CheckInteraction();

		FTimerHandle TimerHandle;

	};
}