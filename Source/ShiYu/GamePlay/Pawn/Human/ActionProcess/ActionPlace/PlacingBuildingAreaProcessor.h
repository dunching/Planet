// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlacingProcessor.h"

class AHumanCharacter;

namespace HumanProcessor
{
	class FPlacingBuildingAreaProcessor : public FPlacingProcessor
	{

		GENERATIONCLASSINFO(FPlacingBuildingAreaProcessor, FPlacingProcessor);

	public:

		const static EInputProcessorType InputProcessprType = EInputProcessorType::kPlaceBuildingArea;

	private:

	};
}