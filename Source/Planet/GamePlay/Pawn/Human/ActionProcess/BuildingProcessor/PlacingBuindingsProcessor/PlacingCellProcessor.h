// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlacingProcessor.h"

class AGroundBase;
class AHumanCharacter;
class ABuildingCaptureData;

namespace HumanProcessor
{
	class FPlacingCellProcessor :  public FPlacingProcessor
	{
		GENERATIONCLASSINFO(FPlacingCellProcessor, FPlacingProcessor);

	public:

		const static EInputProcessorType InputProcessprType = EInputProcessorType::kPlaceCellState;

		FPlacingCellProcessor(FOwnerPawnType* CharacterPtr);

		virtual void MouseLeftPressed()override;

	protected:

		virtual void AttachActor(const FCaptureInfo& CaptureInfo)override;

		TSharedPtr<FCapturesInfo::FCaptureInfo>CaptureInfoSPtr;

	};
}