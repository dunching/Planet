// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/HUD.h"

#include "MyUserWidget.h"

#include "MainHUD.generated.h"

class UMainHUDLayout;
class URegularActionLayout;
class UEndangeredStateLayout;
class UGetItemInfosList;

enum class EMainHUDType : uint8 
{
	kRegularAction,
	kEndangered,
	kNone,
};

UCLASS()
class PLANET_API AMainHUD : public AHUD
{
	GENERATED_BODY()

public:

	virtual void BeginPlay()override;

	virtual void ShowHUD()override;

	void SwitchState(EMainHUDType MainHUDType);

	URegularActionLayout* RegularActionStatePtr = nullptr;
	
protected:
	
	void OnHPChanged(int32 CurrentValue);

	void InitMainHUDLayout();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UMainHUDLayout>MainHUDLayoutClass;
	
	UMainHUDLayout* MainHUDLayoutPtr = nullptr;

	// 常规状态下显示的的HUD	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<URegularActionLayout>RegularActionStateClass;

	// 重伤时要显示的HUD
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UEndangeredStateLayout>EndangeredStateClass;
	
	UEndangeredStateLayout* EndangeredStatePtr = nullptr;

};
