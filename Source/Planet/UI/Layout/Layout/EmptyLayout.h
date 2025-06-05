// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserWidget_Override.h"

#include "GenerateTypes.h"
#include "LayoutInterfacetion.h"
#include "UICommon.h"
#include "MenuInterface.h"
#include "MainMenuCommon.h"

#include "EmptyLayout.generated.h"

class URaffleMenu;
class UTalentAllocation;
class UGroupManaggerMenu;
class UAllocationSkillsMenu;

/**
 *
 */
UCLASS()
class PLANET_API UEmptyLayout :
	public UUserWidget_Override,
	public ILayoutInterfacetion
{
	GENERATED_BODY()

public:

private:

	virtual ELayoutCommon GetLayoutType() const  override final;
	
};
