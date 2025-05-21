// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ViewMenuBaseProcessor.h"

class AHumanCharacter;

namespace HumanProcessor
{
	class FHumanViewSetting : public FViewMenuBaseProcessor
	{
	private:

		GENERATIONCLASSINFO(FHumanViewSetting, FViewMenuBaseProcessor);

	public:

		FHumanViewSetting(FOwnerPawnType* CharacterPtr);

		virtual void EnterAction()override;

		virtual void QuitAction()override;

	protected:

	private:

		void CheckInteraction();

		void QuitCurrentState();

	};
}
