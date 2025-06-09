// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserWidget_Override.h"

#include "GenerateTypes.h"
#include "MenuInterface.h"

#include "MinimapWidget.generated.h"

class UBorder;
class UTextBlock;
class UButton;

/**
 *
 */
UCLASS()
class PLANET_API UMinimapWidget :
	public UUserWidget_Override
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void OnPlayerAngleChangedDelegate(
		float PlayerAngle
		);

	UFUNCTION()
	void OnClickedBtn();

	void OnRegionChanged(
		const FGameplayTag&RegionTag
		);

	void OnHourChanged(
		int32 Hour
		);

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RegionNameText = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TimeText = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* Button = nullptr;

	UPROPERTY(meta = (BindWidget))
	UBorder* PlayerBorder = nullptr;
};
