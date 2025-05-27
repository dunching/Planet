// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserWidget_Override.h"

#include "GenerateTypes.h"

#include "MenuNavgation.generated.h"

class UTalentIcon;

/**
 *
 */
UCLASS()
class PLANET_API UMenuNavgation : public UUserWidget_Override
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

};
