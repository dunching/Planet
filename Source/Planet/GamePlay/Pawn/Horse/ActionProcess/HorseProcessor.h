 // Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "SceneElement.h"
#include "InputProcessor.h"

class ATrackVehicleBase;
class AHorseCharacter;
class ABuildingBase;

namespace HorseProcessor
{
	class FHorseProcessor : public FInputProcessor
	{
	public:

		GENERATIONCLASSINFO(FHorseProcessor, FInputProcessor);

		using FOwnerPawnType = AHorseCharacter;

		const static EInputProcessorType InputProcessprType = EInputProcessorType::kHorseProcessor;

		FHorseProcessor(AHorseCharacter* CharacterPtr);

	};

}