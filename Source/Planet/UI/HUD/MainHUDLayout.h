// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/HUD.h"

#include "UserWidget_Override.h"
#include "LayoutCommon.h"
#include "LayoutInterfacetion.h"
#include "TemplateHelper.h"
#include "UpgradeBoder/UpgradeBoder.h"

#include "MainHUDLayout.generated.h"

class UOverlaySlot;
class UGetItemInfosList;
class UInteractionList;
class UMainMenuLayout;
class UInteractionConversationLayout;
class UInteractionOptionsLayout;
class URegularActionLayout;

UCLASS()
class PLANET_API UMainHUDLayout : public UUserWidget_Override
{
	GENERATED_BODY()

public:
	using FOnLayoutChanged = TCallbackHandleContainer<void(
		ELayoutCommon
		)>;

	virtual void NativeConstruct() override;

	// 
	void SwitchToNewLayout(
		ELayoutCommon LayoutCommon,
		const ILayoutInterfacetion::FOnQuit& OnQuit
		);

	UMainMenuLayout* GetMenuLayout();

	UInteractionConversationLayout* GetConversationLayout();

	UInteractionOptionsLayout* GetInteractionOptionsLayout();

	URegularActionLayout* GetRegularActionLayout() const;

	void DisplayWidgetInOtherCanvas(UUserWidget*WidgetPtr);
	
	// 获取物品的提示
	UGetItemInfosList* GetItemInfos();

	UOverlaySlot* DisplayWidget(
		const TSubclassOf<UUserWidget>& WidgetClass,
		const std::function<void(UUserWidget*)>& Initializer
 
		);

	void RemoveWidget(const TSubclassOf<UUserWidget>& WidgetClass);
	
	void SwitchIsLowerHP(
		bool bIsLowerHP 
		);
	
	// UInteractionList* GetInteractionList();

	FOnLayoutChanged OnLayoutChanged;

protected:
	
};
