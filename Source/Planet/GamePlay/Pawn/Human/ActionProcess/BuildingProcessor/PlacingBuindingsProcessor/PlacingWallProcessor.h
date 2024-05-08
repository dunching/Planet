// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlacingProcessor.h"

class AWallBase;
class AHumanCharacter;

namespace HumanProcessor
{
	class FPlacingWallProcessor :  public FPlacingProcessor
	{
		GENERATIONCLASSINFO(FPlacingWallProcessor, FPlacingProcessor);

	private:

		enum class EWallRot
		{
			kRot1,
			kRot2,
		};

	public:

		const static EInputProcessorType InputProcessprType = EInputProcessorType::kPlaceWallState;

		FPlacingWallProcessor(FOwnerPawnType* CharacterPtr);

		virtual void TKeyPressed()override;

		virtual void MouseLeftPressed()override;

	protected:

		virtual void AttachActor(const FCaptureInfo& CaptureInfo)override;

	private:

		EWallRot WallRot = EWallRot::kRot1;

		TSharedPtr<FCapturesInfo::FCaptureInfo>CaptureInfoSPtr;

	};
}