// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"

#include "RaffleMenu.generated.h"

class UTalentIcon;

/**
 *
 */
UCLASS()
class PLANET_API URaffleMenu : public UMyUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

protected:

};
