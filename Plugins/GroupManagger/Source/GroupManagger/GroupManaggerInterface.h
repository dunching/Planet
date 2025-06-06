// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "GroupManaggerInterface.generated.h"

class AGroupManagger;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGroupManaggerInterface : public UInterface
{
	GENERATED_BODY()
};

class GROUPMANAGGER_API IGroupManaggerInterface
{
	GENERATED_BODY()

public:

	// TODO 
	virtual void OnGroupManaggerReady(AGroupManagger* NewGroupSharedInfoPtr) = 0;

protected:

private:

};
