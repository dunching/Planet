// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/HUD.h"

#include "MyUserWidget.h"
#include "TemplateHelper.h"
#include "LayoutCommon.h"
#include "LayoutInterfacetion.h"

#include "MainHUD.generated.h"

class UMainHUDLayout;
class URegularActionLayout;
class UEndangeredStateLayout;
class UGetItemInfosList;
class UUIManagerSubSystem;
struct FOnAttributeChangeData;

UCLASS()
class PLANET_API AMainHUD : public AHUD
{
	GENERATED_BODY()

public:
	friend UUIManagerSubSystem;

	using FOnInitaliedGroupSharedInfo =
	TCallbackHandleContainer<void()>::FCallbackHandleSPtr;

	virtual void BeginPlay() override;

	virtual void ShowHUD() override;

	void InitalHUD();

protected:
	void SwitchLayout(
		ELayoutCommon MainHUDType,
		const ILayoutInterfacetion::FOnQuit& OnQuit
		);

	UMainHUDLayout* GetMainHUDLayout() const;

	void OnHPChanged(
		const FOnAttributeChangeData&
		);

	void OnHPChangedImp();

	void InitMainHUDLayout();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UMainHUDLayout> MainHUDLayoutClass;

	UMainHUDLayout* MainHUDLayoutPtr = nullptr;
};
