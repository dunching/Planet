// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"
#include "RaffleCommon.h"

#include "RaffleType.generated.h"

class UTalentIcon;

/**
 *
 */
UCLASS()
class PLANET_API URaffleType : public UMyUserWidget
{
	GENERATED_BODY()

public:

	using FOnClicked = TCallbackHandleContainer<void(URaffleType*)>;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	void UnSelect();

	void Select();

	FOnClicked OnClicked;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "RaffleType")
	ERaffleType RaffleType = ERaffleType::kRafflePermanent;

protected:
	
	UFUNCTION()
	void OnBtnClicked();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Display")
	FString DisplayText = TEXT("卡池类型");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Display")
	TSoftObjectPtr<UTexture2D> DefaultIcon;

};
