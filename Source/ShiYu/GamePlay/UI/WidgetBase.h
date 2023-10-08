// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include <GenerateType.h>

#include "WidgetBase.generated.h"

class FHoldItemsData;

/**
 *
 */
UCLASS()
class SHIYU_API UWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void ResetFiled();

	UFUNCTION(BlueprintCallable)
	virtual void DeepClone(UWidgetBase*RightPtr);

private:

};
