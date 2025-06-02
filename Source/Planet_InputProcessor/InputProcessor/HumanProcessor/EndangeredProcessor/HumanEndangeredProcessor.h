// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"

#include "ItemProxy_Minimal.h"
#include "InputProcessor.h"
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
	class PLANET_INPUTPROCESSOR_API FHumanEndangeredProcessor : public FInputProcessor
	{
	private:

		GENERATIONCLASSINFO(FHumanEndangeredProcessor, FInputProcessor);

	public:
		using FOnAllocationChangedHandle =
			TCallbackHandleContainer<void()>::FCallbackHandleSPtr;

		FHumanEndangeredProcessor(FOwnerPawnType* CharacterPtr);

		virtual ~FHumanEndangeredProcessor();

		virtual void EnterAction()override;

		virtual void QuitAction()override;

	};
}