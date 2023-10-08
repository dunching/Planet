// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UI/WidgetBase.h"

#include "ItemType.h"
#include "Pawn/HoldItemComponent.h"

#include "EquipMenu.generated.h"

/**
 *
 */
UCLASS()
class SHIYU_API UEquipMenu : public UWidgetBase
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void ResetFiled()override;

	FItemNum GetItem(int32 SlotNum);

private:

};
