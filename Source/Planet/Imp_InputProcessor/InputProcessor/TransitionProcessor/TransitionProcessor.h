
#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "InputProcessor.h"
#include "ItemProxy_Minimal.h"
#include "GenerateTypes.h"
#include "CharacterBase.h"

class ATrackVehicleBase;
class AHumanCharacter_Player;
class AHorseCharacter;
class ABuildingBase;

namespace HumanProcessor
{
	class PLANET_API FTransitionProcessor : public FInputProcessor
	{
	private:

		GENERATIONCLASSINFO(FTransitionProcessor, FInputProcessor);

	public:

		using FOwnerPawnType = AHumanCharacter_Player;

		FTransitionProcessor(FOwnerPawnType* CharacterPtr);

		virtual void EnterAction()override;

	protected:

	};
}