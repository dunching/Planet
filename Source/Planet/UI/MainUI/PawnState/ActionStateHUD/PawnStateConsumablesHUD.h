// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "CharacterAttributesComponent.h"
#include "UIInterfaces.h"

#include "PawnStateConsumablesHUD.generated.h"

UCLASS()
class PLANET_API UPawnStateConsumablesHUD :
	public UMyUserWidget, 
	public IMenuInterface
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	virtual void ResetUIByData()override;
	
};
