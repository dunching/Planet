// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/HUD.h"

#include "MyUserWidget.h"
#include "LayoutCommon.h"
#include "LayoutInterfacetion.h"

#include "MainHUDLayout.generated.h"

class UGetItemInfosList;
class UInteractionList;
class UMainMenuLayout;
class UInteractionConversationLayout;
class UInteractionOptionsLayout;
class URegularActionLayout;

UCLASS()
class PLANET_API UMainHUDLayout : public UMyUserWidget
{
	GENERATED_BODY()

public:

	using FOnLayoutChanged = TCallbackHandleContainer<void(ELayoutCommon)>;
	
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

	// 获取物品的提示
	UGetItemInfosList* GetItemInfos();

	void SwitchIsLowerHP(
		bool bIsLowerHP
		);

	// UInteractionList* GetInteractionList();

	FOnLayoutChanged OnLayoutChanged;
};
