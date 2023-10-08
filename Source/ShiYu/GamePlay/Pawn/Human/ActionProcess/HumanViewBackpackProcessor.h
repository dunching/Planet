// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "HumanProcessor.h"

class AHumanCharacter;

namespace HumanProcessor
{
	class FViewBackpackProcessor : public FHumanProcessor
	{
	private:

		GENERATIONCLASSINFO(FViewBackpackProcessor, FHumanProcessor);

	public:

		const static EInputProcessorType InputProcessprType = EInputProcessorType::kHumanViewBackpackProcessor;

		FViewBackpackProcessor(AHumanCharacter* CharacterPtr);

		virtual ~FViewBackpackProcessor();

		virtual void EnterAction()override;

		virtual void QuitAction()override;

		virtual void BKeyPressed()override;

		virtual void EKeyPressed()override;

		virtual void ESCKeyPressed()override;

	protected:

	private:

		void CheckInteraction();

		FTimerHandle TimerHandle;

	};
}
