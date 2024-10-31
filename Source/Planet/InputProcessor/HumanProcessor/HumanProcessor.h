
#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "InputProcessor.h"
#include "SceneElement.h"
#include "GenerateType.h"
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

		virtual void MoveForward(const FInputActionValue& InputActionValue)override;

		virtual void MoveRight(const FInputActionValue& InputActionValue)override;

		virtual void AddPitchInput(const FInputActionValue& InputActionValue)override;

		virtual void AddYawInput(const FInputActionValue& InputActionValue)override;

		virtual void LCtrlKeyPressed()override;

		virtual void LShiftKeyPressed()override;

		virtual void LShiftKeyReleased()override;

		virtual void SpaceKeyPressed()override;

		virtual void SpaceKeyReleased()override;

	protected:

		void SwitchWalkingOrRunning();

		void Switch2RegularProcessor();

	};
}