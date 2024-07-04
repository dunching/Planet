// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <mutex>
#include <atomic>

#include "CoreMinimal.h"

#include "HumanProcessor.h"
#include <SceneElement.h>
#include "InteractiveSkillComponent.h"

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

		virtual void MouseLeftReleased()override;

		virtual void MouseRightPressed()override;

		virtual void LAltKeyPressed()override;

		virtual void LAltKeyReleased()override;

	private:

		void AddOrRemoveUseMenuItemEvent(bool bIsAdd);

		bool bIsPressdLeftAlt = false;

		TMap<FKey, TSharedPtr<FCanbeActivedInfo>>HandleKeysMap;

	};

}