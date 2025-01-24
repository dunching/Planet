// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/DragDropOperation.h"

#include "ItemProxy_Minimal.h"
#include "InventoryComponent.h"

#include "ItemProxyDragDropOperation.generated.h"

class UToolIcon;
struct FBasicProxy;
struct FAllocationbleProxy;

/**
 *
 */
UCLASS()
class PLANET_API UItemProxyDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:

	TSharedPtr<FBasicProxy> SceneToolSPtr;

	bool bIsInBackpakc = false;

};

UCLASS()
class PLANET_API UAllocationableProxyDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:

	TSharedPtr<FAllocationbleProxy> SceneToolSPtr;

	bool bIsInBackpakc = false;

};
