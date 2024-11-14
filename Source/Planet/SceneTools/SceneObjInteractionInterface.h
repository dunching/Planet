// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "GenerateType.h"
#include "ItemProxy.h"

#include "SceneObjInteractionInterface.generated.h"

class ACharacterBase;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class USceneObjInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class PLANET_API ISceneObjInteractionInterface
{
	GENERATED_BODY()

public:

	virtual void Interaction(ACharacterBase* CharacterPtr) = 0;

	virtual void StartLookAt(ACharacterBase* CharacterPtr) = 0;

	virtual void LookingAt(ACharacterBase* CharacterPtr) = 0;

	virtual void EndLookAt() = 0;

protected:

private:

};
