// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "HumanViewMenuBase.h"

class AHumanCharacter;

namespace HumanProcessor
{
	class FHumanViewAlloctionSkillsProcessor : public FHumanViewMenuBase
	{
	private:

		GENERATIONCLASSINFO(FHumanViewAlloctionSkillsProcessor, FHumanViewMenuBase);

	public:

		FHumanViewAlloctionSkillsProcessor(FOwnerPawnType* CharacterPtr);

		virtual void EnterAction()override;

		virtual void QuitAction()override;

		virtual void VKeyPressed()override;

		virtual void BKeyPressed()override;

		virtual void ESCKeyPressed()override;

		virtual void PressedNumKey(int32 NumKey)override;

	protected:

	private:

		void CheckInteraction();

		void QuitCurrentState();

	};
}