// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ViewMenuBaseProcessor.h"

class AHumanCharacter;

namespace HumanProcessor
{
	class PLANET_API FViewGroupsProcessor : public FViewMenuBaseProcessor
	{
	private:

		GENERATIONCLASSINFO(FViewGroupsProcessor, FViewMenuBaseProcessor);

	public:

		FViewGroupsProcessor(FOwnerPawnType* CharacterPtr);

		virtual void EnterAction()override;

		virtual void QuitAction()override;

	protected:

	private:

		void CheckInteraction();

		void QuitCurrentState();

	};
}
