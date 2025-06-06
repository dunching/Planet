// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "UserWidget_Override.h"
#include "GenerateTypes.h"
#include "ItemProxy_Minimal.h"
#include "LayoutInterfacetion.h"

#include "CoinList.generated.h"

class UCoinInfo;
struct FCoinProxy;

/**
 *
 */
UCLASS()
class PLANET_API UCoinList :
	public UUserWidget_Override,
	public ILayoutItemInterfacetion
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void Enable() override;

protected:
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UCoinInfo>CoinInfoClass;

};
