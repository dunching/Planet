// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ToolProxyBase.h"

#include "Tool_Base.generated.h"

UCLASS()
class SCENEACTOR_API ATool_Base : public AToolProxyBase
{
	GENERATED_BODY()

public:

	virtual void AttachToCharacter(ACharacter* CharacterPtr)override;

protected:

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

};
