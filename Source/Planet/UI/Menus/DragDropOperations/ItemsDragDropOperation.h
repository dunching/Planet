// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/DragDropOperation.h"

#include "ItemProxy_Minimal.h"
#include "HoldingItemsComponent.h"

#include "ItemsDragDropOperation.generated.h"

class UToolIcon;

/**
 *
 */
UCLASS()
class PLANET_API UItemsDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:

	TSharedPtr<FBasicProxy> SceneToolSPtr;

	bool bIsInBackpakc = false;

};
