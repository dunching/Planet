// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PlanetControllerInterface.generated.h"

class UGroupMnaggerComponent;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UPlanetControllerInterface : public UInterface
{
	GENERATED_BODY()

public:

};


class PLANET_API IPlanetControllerInterface
{
	GENERATED_BODY()

public:

	virtual UGroupMnaggerComponent* GetGroupMnaggerComponent() = 0;

protected:

private:

};
