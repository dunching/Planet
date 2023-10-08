// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlacingProcessor.h"

class AGroundBase;
class AHumanCharacter;
class ABuildingCaptureData;

namespace HumanProcessor
{
	class FActionRoofPlace : public FPlacingProcessor
	{

	public:

		FActionRoofPlace(AHumanCharacter* CharacterPtr);

		const static EInputProcessorType InputProcessprType = EInputProcessorType::kPlaceRoofState;

	protected:

		virtual void AttachActor(const FCaptureInfo& CaptureInfo)override;

	};
}