// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlacingProcessor.h"

class AGroundBase;
class AHumanCharacter;
class ABuildingCaptureData;

namespace HumanProcessor
{
	class FPlacingGroundProcessor :  public FPlacingProcessor
	{
		GENERATIONCLASSINFO(FPlacingGroundProcessor, FPlacingProcessor);

	public:

		const static EInputProcessorType InputProcessprType = EInputProcessorType::kPlaceGroundState;

		FPlacingGroundProcessor(FOwnerPawnType* CharacterPtr);

		virtual void EnterAction()override;

		virtual void QuitAction()override;

		virtual void MouseWheel(const FInputActionValue& InputActionValue)override;

	protected:

		virtual void AttachActor(const FCaptureInfo& CaptureInfo)override;

	private:

		float RotAngle = 0;

	};
}