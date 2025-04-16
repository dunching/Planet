 // Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "ItemProxy_Minimal.h"
#include "HorseProcessor.h"

class ATrackVehicleBase;
class AHorseCharacter;
class AHumanCharacter;
class ABuildingBase;

namespace HorseProcessor
{
	class FHorseRegularProcessor : public FHorseProcessor
	{
	private:

		GENERATIONCLASSINFO(FHorseRegularProcessor, FHorseProcessor);

		enum class EForword
		{
			kForward,
			kNone,
			kBackward,
		};

		enum class ETurn
		{
			kLeft,
			kNone,
			kRight,
		};

	public:

		const static EInputProcessorType InputProcessprType = EInputProcessorType::kHorseRegularProcessor;

		using FHoverUICB = std::function<bool()>;

		FHorseRegularProcessor(AHorseCharacter* CharacterPtr);

		virtual ~FHorseRegularProcessor();

		virtual void EnterAction()override;

		virtual void QuitAction()override;

		virtual void BeginDestroy();

	protected:
	private:

		FTimerHandle TimerHandle;

		bool bIsPressdE = false;

		ABuildingBase* BuildingPtr = nullptr;

		uint8 DestroyBuildingProgress = 0;

	private:

	};
}