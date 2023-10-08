// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UI/WidgetBase.h"

#include "GenerateType.h"
#include "CreateQueue/CreateQueueMG.h"
#include <ItemType.h>

#include "CreateQueue.generated.h"

class APlayerCharacter;
class UMainUI;
class UCreateQueueItem;
class FHoldItemsData;

/**
 *
 */
UCLASS()
class SHIYU_API UCreateQueue : public UWidgetBase
{
	GENERATED_BODY()

public:

	UCreateQueue(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct()override;

	void AddCreateItem(FItemNum ItemsType);

	void SetCreateQueueMG(const TSharedPtr<FCreateQueueMG>& NewSPCreateQueueMGPtr);

protected:

	UFUNCTION(BlueprintCallable)
		void OnAllCancelBtnClick();

private:

	UCreateQueueItem* AddCreateQueueItem(FItemNum NewItemsType);

	TSharedPtr<FCreateQueueMG>SPCreateQueueMGPtr;

	TSharedPtr<FHoldItemsData>SPHoldItemPerpertyPtr;

};
