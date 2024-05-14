// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PlanetControllerInterface.generated.h"

class UGroupMnaggerComponent;
class UGourpmateUnit;
class ACharacterBase;

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

	// “成员管理”记录了成员信息，因为
	virtual UGroupMnaggerComponent* GetGroupMnaggerComponent() = 0;

	virtual UGourpmateUnit* GetGourpMateUnit() = 0;

	virtual ACharacterBase* GetCharacter() = 0;

protected:

private:

};
