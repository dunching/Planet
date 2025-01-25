// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"

#include "ItemProxy_Minimal.h"
#include "HumanProcessor.h"
#include "ProxyProcessComponent.h"

struct FSkillSocketInfo;

class AWeapon_Base;
class ASceneActor;
class AHorseCharacter;
class ISceneActorInteractionInterface;

struct FSocketBase;
struct FWeaponProxy;

namespace HumanProcessor
{
	class FHumanRegularProcessor : public FHumanProcessor
	{
	private:

		GENERATIONCLASSINFO(FHumanRegularProcessor, FHumanProcessor);

	public:
		using FOnAllocationChangedHandle =
			TCallbackHandleContainer<void()>::FCallbackHandleSPtr;

		FHumanRegularProcessor(FOwnerPawnType* CharacterPtr);

		virtual ~FHumanRegularProcessor();

		virtual void EnterAction()override;

		virtual void QuitAction()override;

		virtual void InputKey(const FInputKeyParams& Params)override;

		virtual void F1KeyPressed()override;

		virtual void F2KeyPressed()override;
		
		virtual void F10KeyPressed()override;

		virtual void EKeyPressed()override;

		virtual void EKeyReleased()override;

		virtual void FKeyPressed()override;

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

		virtual void TickImp(float Delta)override;

		void SwitchCurrentWeapon();

		void AddOrRemoveUseMenuItemEvent(bool bIsAdd);

		void UpdateLookAtObject();

		bool bIsPressdLeftAlt = false;

		TMap<FKey, FCharacterSocket>HandleKeysMap;

		FOnAllocationChangedHandle OnAllocationChangedHandle;

	};
}