// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ChallengeEntry.h"
#include "HumanCharacter_AI.h"
#include "HumanInteractionBase.h"

#include "InputProcessor.h"

class AHumanCharacter_Player;
class AHumanCharacter_AI;
class AChallengeEntry;

namespace HumanProcessor
{
	class FHumanInteractionWithChallengeEntryProcessor : public FHumanInteractionBaseProcessor
	{
	private:

		GENERATIONCLASSINFO(FHumanInteractionWithChallengeEntryProcessor, FHumanInteractionBaseProcessor);

	public:

		using FOwnerPawnType = AHumanCharacter_Player;

		FHumanInteractionWithChallengeEntryProcessor(FOwnerPawnType* CharacterPtr);

		virtual void EnterAction()override;

		virtual void QuitAction() override;

		TObjectPtr<AChallengeEntry>TargetPtr = nullptr;
		
	protected:

		void Switch2RegularProcessor();

	};
}
