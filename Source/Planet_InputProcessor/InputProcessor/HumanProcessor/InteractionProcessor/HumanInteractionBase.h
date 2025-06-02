// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HumanCharacter_AI.h"

#include "InputProcessor.h"

class AHumanCharacter_Player;
class AHumanCharacter_AI;
class ISceneActorInteractionInterface;

namespace HumanProcessor
{
	class PLANET_INPUTPROCESSOR_API FHumanInteractionBaseProcessor : public FInputProcessor
	{
	private:
		GENERATIONCLASSINFO(FHumanInteractionBaseProcessor, FInputProcessor);

	public:
		using FOwnerPawnType = APawn;

		FHumanInteractionBaseProcessor(
			FOwnerPawnType* CharacterPtr
			);

		virtual bool InputKey(
			const FInputKeyEventArgs& EventArgs
			) override;

		void StopInteraciton();
		
	protected:
		
		void Switch2RegularProcessor();
		
		ISceneActorInteractionInterface* SceneActorInteractionInterfacePtr = nullptr;
	};
}
