// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"

#include "UserWidget_Override.h"
#include "ItemProxy_Minimal.h"
#include "InventoryComponent.h"
#include "ProxyProcessComponent.h"

#include "ToolsMenu.generated.h"

class UToolIcon;
class UToolIcon;

/**
 *
 */
UCLASS()
class PLANET_API UToolsMenu : public UUserWidget_Override
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

private:

};
