// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PlanetControllerInterface.generated.h"

class UGroupMnaggerComponent;
class UPlanetAbilitySystemComponent;
class UCharacterUnit;
class AHumanCharacter;

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

	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const = 0;

	// “成员管理”记录了成员信息，因为
	virtual UGroupMnaggerComponent* GetGroupMnaggerComponent() const = 0;

	virtual UCharacterUnit* GetGourpMateUnit() = 0;

	virtual void ResetGroupmateUnit(UCharacterUnit* NewGourpMateUnitPtr) = 0;

protected:

private:

};
