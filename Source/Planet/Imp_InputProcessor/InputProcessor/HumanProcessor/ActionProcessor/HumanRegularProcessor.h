// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "GameOptions.h"
#include "GameplayAbilitySpecHandle.h"

#include "ItemProxy_Minimal.h"
#include "HumanProcessor.h"
#include "ProxyProcessComponent.h"

struct FSkillSocketInfo;

class APlanetWeapon_Base;
class ASceneActor;
class AHorseCharacter;
class ISceneActorInteractionInterface;

struct FSocketBase;
struct FWeaponProxy;

namespace HumanProcessor
{
	class PLANET_API FHumanRegularProcessor : public FHumanProcessor
	{
	private:
		GENERATIONCLASSINFO(FHumanRegularProcessor, FHumanProcessor);

	public:
		using FOnAllocationChangedHandle =
		TCallbackHandleContainer<void()>::FCallbackHandleSPtr;

		FHumanRegularProcessor(
			FOwnerPawnType* CharacterPtr
			);

		virtual ~FHumanRegularProcessor();

		virtual void EnterAction() override;

		virtual void QuitAction() override;

	protected:
		virtual bool InputKey(
			const FInputKeyEventArgs& EventArgs
			) override;

		virtual bool InputAxis(
			const FInputKeyEventArgs& EventArgs
			) override;

	private:
		virtual void TickImp(
			float Delta
			) override;

		void SwitchWeapon();

		void SwitchCurrentWeapon();

		void AddOrRemoveUseMenuItemEvent(
			bool bIsAdd
			);

		void UpdateLookAtObject();

		bool bIsPressdLeftAlt = false;

		TMap<FKey, FCharacterSocket> HandleKeysMap;

		FOnAllocationChangedHandle OnAllocationChangedHandle;
	};
}
