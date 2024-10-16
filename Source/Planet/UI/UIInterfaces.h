// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "GenerateType.h"
#include "SceneElement.h"

#include "UIInterfaces.generated.h"

struct FBasicProxy;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UUnitIconInterface : public UInterface
{
	GENERATED_BODY()
};

class PLANET_API IUnitIconInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	virtual void InvokeReset(UUserWidget* BaseWidgetPtr) = 0;
	
	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicUnitPtr) = 0;

	virtual void EnableIcon(bool bIsEnable) = 0;

protected:

private:

};
