// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/HUD.h"

#include "MyUserWidget.h"

#include "MainHUD.generated.h"

class UMainHUDLayout;
class UGetItemInfosList;

UCLASS()
class PLANET_API AMainHUD : public AHUD
{
	GENERATED_BODY()

public:

	virtual void BeginPlay()override;

	virtual void ShowHUD()override;

	void OnHPChanged(int32 CurrentValue);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UMainHUDLayout>MainHUDLayoutClass;
	
	UMainHUDLayout* MainHUDLayoutPtr = nullptr;

};
