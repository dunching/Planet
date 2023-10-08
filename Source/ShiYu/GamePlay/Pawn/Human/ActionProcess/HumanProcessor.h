// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "ItemType.h"
#include "InputProcessor.h"

class AHumanCharacter;

namespace HumanProcessor
{
	class FHumanProcessor : public FInputProcessor
	{
	public:

		GENERATIONCLASSINFO(FHumanProcessor, FInputProcessor);

		const static EInputProcessorType InputProcessprType = EInputProcessorType::kHumanProcessor;

		using FOwnerPawnType = AHumanCharacter;

		FHumanProcessor(AHumanCharacter* CharacterPtr);

	};

}