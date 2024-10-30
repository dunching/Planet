// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"

#include "SceneElement.h"
#include "HumanProcessor.h"
#include "ProxyProcessComponent.h"

struct FSkillSocketInfo;

class AWeapon_Base;
class ASceneObj;
class AHorseCharacter;
class ISceneObjInteractionInterface;

struct FSocketBase;
struct FWeaponProxy;

namespace HumanProcessor
{
	class FHumanEndangeredProcessor : public FHumanProcessor
	{
	private:

		GENERATIONCLASSINFO(FHumanEndangeredProcessor, FHumanProcessor);

	public:
		using FOnAllocationChangedHandle =
			TCallbackHandleContainer<void()>::FCallbackHandleSPtr;

		FHumanEndangeredProcessor(FOwnerPawnType* CharacterPtr);

		virtual ~FHumanEndangeredProcessor();

		virtual void EnterAction()override;

		virtual void QuitAction()override;

	};
}