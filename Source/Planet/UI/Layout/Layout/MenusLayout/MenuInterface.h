// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "GenerateType.h"
#include "ItemProxy_Minimal.h"
#include "MainMenuCommon.h"

#include "MenuInterface.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UMenuInterface : public UInterface
{
	GENERATED_BODY()
};

class PLANET_API IMenuInterface
{
	GENERATED_BODY()

public:

	virtual void EnableMenu() = 0;
	
	virtual void DisEnableMenu() = 0;

	virtual EMenuType GetMenuType()const = 0;
	
protected:

private:

};
