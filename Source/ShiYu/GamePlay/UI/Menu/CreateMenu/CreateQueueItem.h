// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UI/WidgetBase.h"

#include "CreateQueue/CreateCommonType.h"
#include "Common/GenerateType.h"
#include <ItemType.h>

#include "CreateQueueItem.generated.h"

class FCreateQueueMG;
class FCreateRequest;
class FHoldItemsData;

/**
 *
 */
UCLASS()
class SHIYU_API UCreateQueueItem : public UWidgetBase
{
	GENERATED_BODY()

public:

	UCreateQueueItem(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct()override;

	void SetItemType(FItemNum NewItemsType);

	FItemNum GetItemType()const;

	void OnCreateComplete();

	void UpdateProgress(ECreateState CreateState, float Progress);

	UFUNCTION(BlueprintCallable)
		void OnCancelBtnClick();

	void SetCreateQueueMG(const TSharedPtr<FCreateQueueMG>& NewSPCreateQueueMGPtr);

	void SetHoldItemProperty(const TSharedPtr<FHoldItemsData>& NewSPHoldItemPerperty);

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Val)
		FItemNum ItemsType;

private:

	TSharedPtr<FCreateRequest>SPCreateRequestPtr;

	TSharedPtr<FCreateQueueMG>SPCreateQueueMGPtr;

	TSharedPtr<FHoldItemsData>SPHoldItemPerpertyPtr;

};
