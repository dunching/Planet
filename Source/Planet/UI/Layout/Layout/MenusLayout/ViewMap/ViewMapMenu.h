// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserWidget_Override.h"

#include "GenerateTypes.h"
#include "MenuInterface.h"

#include "ViewMapMenu.generated.h"

/**
 *
 */
UCLASS()
class PLANET_API UViewMapMenu :
	public UUserWidget_Override,
	public IMenuInterface
{
	GENERATED_BODY()

public:

protected:

	virtual void EnableMenu()override;

	virtual void DisEnableMenu()override;

	virtual EMenuType GetMenuType()const override final;
};
