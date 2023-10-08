// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"

#include "Common/GenerateType.h"

#include "CreateQueue/CreateQueueMG.h"
#include "CreateMenu.generated.h"

class UCreateQueue;
class FHoldItemsData;

/**
 *
 */
UCLASS()
class SHIYU_API UCreateMenu : public UWidgetBase
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void ResetFiled()override;

	void SetHoldItemProperty(TSharedPtr<FHoldItemsData>NewHoldItemPerpertyPtr);

private:

	void InitCreateQueue();

	void InitCreateTile();

	TSharedPtr<FHoldItemsData>SPHoldItemPerpertyPtr;

	UCreateQueue* CreateQueuePtr = nullptr;

	TSharedPtr<FCreateQueueMG>SPCreateQueueMGPtr;

};
