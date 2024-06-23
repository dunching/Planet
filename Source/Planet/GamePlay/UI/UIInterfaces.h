// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "GenerateType.h"
#include "SceneElement.h"

#include "UIInterfaces.generated.h"

class UBasicUnit;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UToolsIconInterface : public UInterface
{
	GENERATED_BODY()
};

class PLANET_API IToolsIconInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	virtual void InvokeReset(UUserWidget* BaseWidgetPtr) = 0;
	
	virtual void ResetToolUIByData(UBasicUnit * BasicUnitPtr) = 0;

	virtual void EnableIcon(bool bIsEnable) = 0;

protected:

private:

};

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UItemsMenuInterface : public UInterface
{
	GENERATED_BODY()
};


class PLANET_API IItemsMenuInterface
{
	GENERATED_BODY()

public:

	virtual void ResetUIByData() = 0;

protected:

private:

};
