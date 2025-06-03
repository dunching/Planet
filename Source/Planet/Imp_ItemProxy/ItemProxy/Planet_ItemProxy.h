// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemProxy.h"
#include "PAD_ItemProxyCollection.h"

#include "Planet_ItemProxy.generated.h"

class UInventoryComponent;

USTRUCT()
struct PLANET_API FPlanet_BasicProxy : public FBasicProxy
{
	GENERATED_BODY()

public:
	void SetInventoryComponent(const TObjectPtr<UInventoryComponent>&InventoryComponentPtr);

	TObjectPtr<UInventoryComponent> GetInventoryComponent() const;

};