// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "GenerateType.h"
#include "ItemProxy_Minimal.h"

#include "GroupSharedInterface.generated.h"

class AGroupSharedInfo;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGroupSharedInterface : public UInterface
{
	GENERATED_BODY()
};

class PLANET_API IGroupSharedInterface
{
	GENERATED_BODY()

public:

	virtual void OnGroupSharedInfoReady(AGroupSharedInfo* NewGroupSharedInfoPtr) = 0;

protected:

private:

};
