// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "HorseProcessor.h"

class AHorseCharacter;

namespace HorseProcessor
{
	class FHorseViewBackpackProcessor : public FHorseProcessor
	{
	private:

		GENERATIONCLASSINFO(FHorseViewBackpackProcessor, FHorseProcessor);

	public:

		const static EInputProcessorType InputProcessprType = EInputProcessorType::kHorseViewBackpackProcessor;

		FHorseViewBackpackProcessor(AHorseCharacter* CharacterPtr);

		virtual ~FHorseViewBackpackProcessor();

		AHorseCharacter* GetOwnerActor() { return Cast<AHorseCharacter>(OnwerPawnPtr); }

		virtual void EnterAction()override;

		virtual void QuitAction()override;

		virtual void BKeyPressed()override;

		virtual void EKeyPressed()override;

		virtual void ESCKeyPressed()override;

	protected:

	private:

		void CheckInteraction();

	};
}
