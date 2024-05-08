// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <mutex>
#include <atomic>

#include "CoreMinimal.h"

#include "HumanProcessor.h"
#include <SceneElement.h>
#include "EquipmentElementComponent.h"

class AHumanCharacter;
class ABuildingBase;
class AEquipmentBase;
class ABuildingCaptureData;
class UBuildInteractionWidgetBase;
class UPlaceWidgetBase;
class UToolIcon;
class ATool_Base;

struct FSceneTool;

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

		virtual void InputKey(const FInputKeyParams& Params)override;

		virtual void QKeyPressed()override;

		virtual void VKeyPressed()override;

		virtual void BKeyPressed()override;

		virtual void MouseLeftPressed()override;

		virtual void MouseLeftReleased();

		virtual void MouseRightPressed()override;

	private:

		void AddOrRemoveUseMenuItemEvent(bool bIsAdd);
		
		TMap<FKey, FToolsSocketInfo>HandleKeysMap;

		ATool_Base* CurrentEquipmentPtr = nullptr;

		UToolUnit::IDType PreviousID = 0;

	};

}