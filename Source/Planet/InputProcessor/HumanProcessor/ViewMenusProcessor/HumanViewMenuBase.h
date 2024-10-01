// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "InputProcessor.h"

class AHumanCharacter;

namespace HumanProcessor
{
	class FHumanViewMenuBase : public FInputProcessor
	{
	private:

		GENERATIONCLASSINFO(FHumanViewMenuBase, FInputProcessor);

	public:

		using FOwnerPawnType = AHumanCharacter;

		FHumanViewMenuBase(FOwnerPawnType* CharacterPtr);

		virtual void ESCKeyPressed()override;

	protected:

		void Switch2RegularProcessor();

	};
}
