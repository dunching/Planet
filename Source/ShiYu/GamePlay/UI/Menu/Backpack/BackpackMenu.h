// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UI/WidgetBase.h"

#include "Common/GenerateType.h"
#include "Pawn/HoldItemComponent.h"
#include <ItemType.h>

#include "BackpackMenu.generated.h"

/**
 *
 */
UCLASS()
class SHIYU_API UBackpackMenu : public UWidgetBase
{
	GENERATED_BODY()

public:

	using FOnItemChangeCB = std::function<void(FItemAry, EItemChangeType)>;

	enum class EPutPostion
	{
		kLeft,
		kRight,
	};

	virtual void NativeConstruct()override;

	virtual void ResetFiled()override;

	void SetHoldItemProperty(const TSharedPtr<FHoldItemsData>& NewSPHoldItemPerperty);

	void SetPutPostion(EPutPostion PutPostion);

private:

	TSharedPtr<FHoldItemsData>SPHoldItemPerpertyPtr;

	FOnItemChangeCB OnItemChangeCB;

};
