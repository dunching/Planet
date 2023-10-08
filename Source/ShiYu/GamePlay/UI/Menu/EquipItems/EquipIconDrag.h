// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/DragDropOperation.h"

#include <ItemType.h>

#include "EquipIconDrag.generated.h"

class UEquipIcon;

/**
 *
 */
UCLASS()
class SHIYU_API UEquipIconDrag : public UDragDropOperation
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = DragDropOperation, meta = (AllowPrivateAccess = "true", ExposeOnSpawn = "true"))
		FItemNum ItemPropertyBase;

};
