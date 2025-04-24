
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "GenerateType.h"
#include "LayoutCommon.h"

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

	virtual void Enable();
	
	virtual void DisEnable();

	virtual ELayoutCommon GetLayoutType() const = 0;
	
protected:

	bool bIsActive = false;
	
private:

};
