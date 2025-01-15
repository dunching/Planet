// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/HUD.h"

#include "MyUserWidget.h"

#include "MainHUDLayout.generated.h"

class UGetItemInfosList;
class UInteractionList;

UCLASS()
class PLANET_API UMainHUDLayout : public UMyUserWidget
{
	GENERATED_BODY()

public:

	// 
	void SwitchToNewLayout(ELayoutCommon LayoutCommon);

	// 获取物品的提示
	UGetItemInfosList* GetItemInfos();

	void SwitchIsLowerHP(bool bIsLowerHP);

	UInteractionList* GetInteractionList();

};
