
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
	class FHumanProcessor : public FInputProcessor
	{
	private:

		GENERATIONCLASSINFO(FHumanProcessor, FInputProcessor);

		enum class EForword
		{
			kForward,
			kNone,
			kBackward,
		};

		enum class ETurn
		{
			kLeft,
			kNone,
			kRight,
		};

	public:

		using FOwnerPawnType = AHumanCharacter_Player;

		FHumanProcessor(FOwnerPawnType* CharacterPtr);

		virtual void EnterAction()override;

	protected:

		virtual bool InputKey(
			const FInputKeyEventArgs& EventArgs
		) override;

		void SwitchWalkingOrRunning();

		void Dash();

		void Jump();

	};
}