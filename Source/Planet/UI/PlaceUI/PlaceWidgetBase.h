// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "PlaceWidgetBase.generated.h"

/**
 *
 */
UCLASS()
class PLANET_API UPlaceWidgetBase : public UUserWidget_Override
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	void SetPromtStr(const FString& PromPtStr);

	void DisablePromt();

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName BorderName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName TextName;

};
