// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <mutex>
#include <atomic>

#include "CoreMinimal.h"

#include "HumanProcessor.h"
#include "ItemProxy_Minimal.h"
#include "ProxyProcessComponent.h"

class AHumanCharacter;
class ABuildingBase;
class AEquipmentBase;
class ABuildingCaptureData;
class UBuildInteractionWidgetBase;
class UPlaceWidgetBase;
class UToolIcon;
class ATool_Base;

struct FSceneTool;
struct FSocketBase;

namespace HumanProcessor
{
	class FBuildingBaseProcessor : public FHumanProcessor
	{
	private:

	public:

		GENERATIONCLASSINFO(FBuildingBaseProcessor, FHumanProcessor);

		FBuildingBaseProcessor(FOwnerPawnType* CharacterPtr);

		virtual void EnterAction()override;

		virtual void QuitAction()override;

	public:
	private:

		void AddOrRemoveUseMenuItemEvent(bool bIsAdd);

		bool bIsPressdLeftAlt = false;

	};

}