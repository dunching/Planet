// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "ItemProxy_Minimal.h"

#include "SceneActorInteractionInterface.generated.h"

class ASceneActor;
class ACharacterBase;
class USceneActorInteractionComponent;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class USceneActorInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class PLANET_API ISceneActorInteractionInterface
{
	GENERATED_BODY()

public:

	/**
	 * 开始交互
	 * @param CharacterPtr 
	 */
	virtual void HasbeenInteracted(ACharacterBase* CharacterPtr) = 0;

	virtual void HasBeenStartedLookAt(ACharacterBase* CharacterPtr) = 0;

	virtual void HasBeenLookingAt(ACharacterBase* CharacterPtr) = 0;

	virtual void HasBeenEndedLookAt() = 0;

	virtual USceneActorInteractionComponent*GetSceneActorInteractionComponent()const = 0;
	
protected:
	
private:

};
