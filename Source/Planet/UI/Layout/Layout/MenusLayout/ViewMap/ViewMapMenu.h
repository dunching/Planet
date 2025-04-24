// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"
#include "MenuInterface.h"

#include "ViewMapMenu.generated.h"

/**
 *
 */
UCLASS()
class PLANET_API UViewMapMenu :
	public UMyUserWidget,
	public IMenuInterface
{
	GENERATED_BODY()

public:

protected:

	virtual void ResetUIByData()override;

	virtual void SyncData()override;

	virtual EMenuType GetMenuType()const override final;
};
