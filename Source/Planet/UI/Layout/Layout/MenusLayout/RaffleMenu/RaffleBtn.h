// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserWidget_Override.h"

#include "GenerateTypes.h"
#include "TemplateHelper.h"

#include "RaffleBtn.generated.h"

class UTalentIcon;

/**
 *
 */
UCLASS()
class PLANET_API URaffleBtn : public UUserWidget_Override
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
	FString DisplayText = TEXT("�鿨����");

};
