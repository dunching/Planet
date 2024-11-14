 // Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "ItemProxy.h"
#include "HorseProcessor.h"

class ATrackVehicleBase;
class AHorseCharacter;
class ABuildingBase;

namespace HorseProcessor
{
	class FAICtrlHorseProcessor : public FHorseProcessor
	{
	private:

		GENERATIONCLASSINFO(FAICtrlHorseProcessor, FHorseProcessor);

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

		const static EInputProcessorType InputProcessprType = EInputProcessorType::kHorseAICtrlProcessor;

		using FHoverUICB = std::function<bool()>;

		FAICtrlHorseProcessor(AHorseCharacter* CharacterPtr);

		virtual ~FAICtrlHorseProcessor();

		virtual void EnterAction()override;

		virtual void QuitAction()override;

		virtual void BeginDestroy();

		virtual void MoveForward(const FInputActionValue& InputActionValue)override;

		virtual void MoveRight(const FInputActionValue& InputActionValue)override;

		virtual void MoveUp(const FInputActionValue& InputActionValue)override;

		virtual void MouseWheel(const FInputActionValue& InputActionValue)override;

		virtual void AddPitchInput(const FInputActionValue& InputActionValue)override;

		virtual void AddYawInput(const FInputActionValue& InputActionValue)override;

		virtual void WKeyPressed()override;

		virtual void WKeyReleased()override;

		virtual void AKeyPressed()override;

		virtual void AKeyReleased()override;

		virtual void SKeyPressed()override;

		virtual void SKeyReleased()override;

		virtual void DKeyPressed()override;

		virtual void DKeyReleased()override;

		virtual void EKeyPressed()override;

		virtual void EKeyReleased()override;

		virtual void RKeyPressed()override;

		virtual void TKeyPressed()override;

		virtual void YKeyPressed()override;

		virtual void FKeyPressed()override;

		virtual void BKeyPressed()override;

		virtual void GKeyPressed()override;

		virtual void ESCKeyPressed()override;

		virtual void LCtrlKeyPressed()override;

		virtual void LCtrlKeyReleased()override;

		virtual void LShiftKeyPressed()override;

		virtual void LShiftKeyReleased()override;

		virtual void SpaceKeyPressed()override;

		virtual void SpaceKeyReleased()override;

		virtual void MouseLeftReleased()override;

		virtual void MouseLeftPressed()override;

		virtual void MouseRightReleased()override;

		virtual void MouseRightPressed()override;

		virtual void PressedNumKey(int32 NumKey)override;

	protected:

	private:

		void UpdateLookAtObject();

		FTimerHandle TimerHandle;

		bool bIsPressdE = false;

		ABuildingBase* BuildingPtr = nullptr;

		uint8 DestroyBuildingProgress = 0;

	private:

	};

}