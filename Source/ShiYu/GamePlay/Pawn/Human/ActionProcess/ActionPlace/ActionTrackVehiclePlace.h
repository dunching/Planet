// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlacingProcessor.h"

class ATrackVehicleBase;
class AHumanCharacter;

namespace HumanProcessor
{
	class FActionTrackVehiclePlace : public FPlacingProcessor
	{

	public:

		FActionTrackVehiclePlace(AHumanCharacter* CharacterPtr);

	protected:

	private:

		virtual void OnPlaceItemPrev()override;

		bool PrevFindLine(const FHitResult& OutHit, const FVector& StartPt, const FVector& StopPt);

	};
}