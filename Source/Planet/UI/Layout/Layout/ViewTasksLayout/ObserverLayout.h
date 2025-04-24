// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "GenerateType.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"


#include "ObserverLayout.generated.h"

class UToolsMenu;

struct FCharacterAttributes;

struct FToolsSocketInfo;
struct FConsumableSocketInfo;

/**
 *
 */
UCLASS()
class PLANET_API UObserverLayout :
	public UMyUserWidget,
	public ILayoutInterfacetion
{
	GENERATED_BODY()

public:

private:

	virtual ELayoutCommon GetLayoutType() const  override final;
	
};
