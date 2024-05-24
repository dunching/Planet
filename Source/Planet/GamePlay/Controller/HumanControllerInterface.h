// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "HumanControllerInterface.generated.h"

class UGroupMnaggerComponent;
class UGourpmateUnit;
class AHumanCharacter;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UHumanControllerInterface : public UInterface
{
	GENERATED_BODY()

public:

};

class PLANET_API IHumanControllerInterface
{
	GENERATED_BODY()

public:

	using FPawnType = AHumanCharacter;

	// “成员管理”记录了成员信息，因为
	virtual UGroupMnaggerComponent* GetGroupMnaggerComponent() const = 0;

	virtual UGourpmateUnit* GetGourpMateUnit() = 0;

protected:

private:

};
