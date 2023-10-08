// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlacingProcessor.h"

class ATrackBase;
class AHumanCharacter;

namespace HumanProcessor
{
	class FActionTrackPlace : public FPlacingProcessor
	{

	public:

		FActionTrackPlace(AHumanCharacter* CharacterPtr);

		virtual void EnterAction()override;

		virtual void MouseLeftPressed()override;

	protected:

	private:

		virtual void OnPlaceItemPrev()override;

		bool PrevFindSphere(const TArray<FHitResult>& OutHitsAry, const FVector& StartPt, const FVector& StopPt);

		bool PrevFindLine(const FHitResult& OutHit, const FVector& StartPt, const FVector& StopPt);

		FTransform GetPreviousPt();

		ATrackBase* TrackPtr = nullptr;

		ATrackBase* LinkTrackPtr = nullptr;

		ETrackVehiclePosState LinkPos = ETrackVehiclePosState::kFront;

		int32 CurPtIndex = 0;

		float FindArea = 100;

	};
}