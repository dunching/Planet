
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "GenerateType.h"
#include "ItemProxy_Minimal.h"

#include "LayoutInterfacetion.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class ULayoutInterfacetion : public UInterface
{
	GENERATED_BODY()
};

class PLANET_API ILayoutInterfacetion
{
	GENERATED_BODY()

public:

	virtual void Enable() = 0;
	
	virtual void DisEnable() = 0;

protected:

private:

};
