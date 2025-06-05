// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "CharacterAttributesComponent.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"

#include "PawnStateConsumablesHUD.generated.h"

UCLASS()
class PLANET_API UPawnStateConsumablesHUD :
	public UUserWidget_Override, 
	public ILayoutItemInterfacetion
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void Enable() override;

	virtual void DisEnable() override;

};
