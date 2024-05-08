// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"

#include "SceneElement.h"
#include "Pawn/HoldingItemsComponent.h"
#include "EquipmentElementComponent.h"

#include "ToolsMenu.generated.h"

class UToolIcon;
class UToolIcon;

/**
 *
 */
UCLASS()
class PLANET_API UToolsMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

private:

};
