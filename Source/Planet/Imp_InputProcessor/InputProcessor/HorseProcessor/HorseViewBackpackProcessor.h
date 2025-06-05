// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "HorseProcessor.h"

class AHorseCharacter;

namespace HorseProcessor
{
	class PLANET_API FHorseViewBackpackProcessor : public FHorseProcessor
	{
	private:

		GENERATIONCLASSINFO(FHorseViewBackpackProcessor, FHorseProcessor);

	public:

		FHorseViewBackpackProcessor(AHorseCharacter* CharacterPtr);

		virtual ~FHorseViewBackpackProcessor();

		AHorseCharacter* GetOwnerActor();

		virtual void EnterAction()override;

		virtual void QuitAction()override;

	protected:

	private:

		void CheckInteraction();

	};
}
