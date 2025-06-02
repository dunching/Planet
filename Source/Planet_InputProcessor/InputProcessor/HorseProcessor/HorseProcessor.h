 // Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "ItemProxy_Minimal.h"
#include "InputProcessor.h"

class ATrackVehicleBase;
class AHorseCharacter;
class ABuildingBase;

namespace HorseProcessor
{
	class PLANET_INPUTPROCESSOR_API FHorseProcessor : public FInputProcessor
	{
	public:

		GENERATIONCLASSINFO(FHorseProcessor, FInputProcessor);

		using FOwnerPawnType = AHorseCharacter;

		FHorseProcessor(AHorseCharacter* CharacterPtr);

	};

}