// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "GenerateType.h"
#include "ItemProxy_Minimal.h"

#include "SceneActorInteractionInterface.generated.h"

class ACharacterBase;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class USceneActorInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class PLANET_API ISceneActorInteractionInterface
{
	GENERATED_BODY()

public:

	virtual void HasbeenInteracted(ACharacterBase* CharacterPtr) = 0;

	virtual void HasBeenStartedLookAt(ACharacterBase* CharacterPtr) = 0;

	virtual void HasBeenLookingAt(ACharacterBase* CharacterPtr) = 0;

	virtual void HasBeenEndedLookAt() = 0;

protected:

private:

};
