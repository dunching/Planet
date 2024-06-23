// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/DragDropOperation.h"

#include <SceneElement.h>
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

	UBasicUnit* SceneToolSPtr;

	std::function<void(bool)>OnDroped;

	bool bIsInBackpakc = false;

};
