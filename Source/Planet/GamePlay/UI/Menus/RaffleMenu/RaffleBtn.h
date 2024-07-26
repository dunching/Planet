// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"

#include "RaffleBtn.generated.h"

class UTalentIcon;

/**
 *
 */
UCLASS()
class PLANET_API URaffleBtn : public UMyUserWidget
{
	GENERATED_BODY()

public:

	using FOnClicked = TCallbackHandleContainer<void(URaffleBtn*)>;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	FOnClicked OnClicked;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "RaffleType")
	int32 ReffleCount = 1;

protected:
	
	UFUNCTION()
	void OnBtnClicked();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Display")
	FString DisplayText = TEXT("³é¿¨´ÎÊý");

};
