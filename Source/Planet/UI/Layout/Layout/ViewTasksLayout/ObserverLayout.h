// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "GenerateTypes.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"


#include "ObserverLayout.generated.h"

class UToolsMenu;



struct FToolsSocketInfo;
struct FConsumableSocketInfo;

/**
 *
 */
UCLASS()
class PLANET_API UObserverLayout :
	public UUserWidget_Override,
	public ILayoutInterfacetion
{
	GENERATED_BODY()

public:

private:

	virtual ELayoutCommon GetLayoutType() const  override final;
	
};
