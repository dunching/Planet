// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "HumanControllerInterface.generated.h"

class UGroupMnaggerComponent;
class UPlanetAbilitySystemComponent;
class UGourpmateUnit;
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

	using FPawnType = AHumanCharacter;

	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const = 0;

	// “成员管理”记录了成员信息，因为
	virtual UGroupMnaggerComponent* GetGroupMnaggerComponent() const = 0;

	virtual UGourpmateUnit* GetGourpMateUnit() = 0;

protected:

private:

};
