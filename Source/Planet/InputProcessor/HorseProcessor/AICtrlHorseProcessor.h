 // Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "ItemProxy_Minimal.h"
#include "HorseProcessor.h"

class ATrackVehicleBase;
class AHorseCharacter;
class ABuildingBase;

namespace HorseProcessor
{
	class FAICtrlHorseProcessor : public FHorseProcessor
	{
	private:

		GENERATIONCLASSINFO(FAICtrlHorseProcessor, FHorseProcessor);

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

		using FHoverUICB = std::function<bool()>;

		FAICtrlHorseProcessor(AHorseCharacter* CharacterPtr);

		virtual ~FAICtrlHorseProcessor();

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