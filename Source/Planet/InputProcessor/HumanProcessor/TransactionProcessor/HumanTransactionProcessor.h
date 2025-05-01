// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"

#include "ItemProxy_Minimal.h"
#include "InputProcessor.h"
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
	class FHumanTransactionProcessor : public FInputProcessor
	{
	private:

		GENERATIONCLASSINFO(FHumanTransactionProcessor, FInputProcessor);

	public:
		using FOnAllocationChangedHandle =
			TCallbackHandleContainer<void()>::FCallbackHandleSPtr;

		FHumanTransactionProcessor(FOwnerPawnType* CharacterPtr);

		virtual void EnterAction()override;

		virtual void QuitAction()override;

		virtual bool InputKey(
			const FInputKeyEventArgs& EventArgs
		) override;

		AHumanCharacter_AI* CharacterPtr = nullptr;
		
	};
}