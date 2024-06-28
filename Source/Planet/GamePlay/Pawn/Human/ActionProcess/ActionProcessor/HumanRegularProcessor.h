// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"

#include "SceneElement.h"
#include "HumanProcessor.h"
#include "EquipmentElementComponent.h"

struct FSkillSocketInfo;

class AWeapon_Base;

class UWeaponUnit;

namespace HumanProcessor
{
	class FHumanRegularProcessor : public FHumanProcessor
	{
	private:

		GENERATIONCLASSINFO(FHumanRegularProcessor, FHumanProcessor);

	public:

		FHumanRegularProcessor(FOwnerPawnType* CharacterPtr);

		virtual ~FHumanRegularProcessor();

		virtual void EnterAction()override;

		virtual void QuitAction()override;

		virtual void InputKey(const FInputKeyParams& Params)override;

		virtual void F1KeyPressed()override;

		virtual void F2KeyPressed()override;

		virtual void QKeyPressed()override;

		virtual void GKeyPressed()override;

		virtual void HKeyPressed()override;

		virtual void XKeyPressed()override;

		virtual void VKeyPressed()override;

		virtual void BKeyPressed()override;

		virtual void TabKeyPressed()override;

		virtual void LAltKeyPressed()override;

		virtual void LAltKeyReleased()override;

	private:

		void SwitchCurrentWeapon();

		void AddOrRemoveUseMenuItemEvent(bool bIsAdd);

		void UpdateLookAtObject();

		TMap<FKey, TSharedPtr<FCanbeActivedInfo>>HandleKeysMap;

	};
}