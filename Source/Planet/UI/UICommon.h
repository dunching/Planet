// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UICommon.generated.h"

enum EUIOrder : int8
{
	// WidgetComponent默认为-100
	kUIOrderDefault = -99,

	kConversationBorder = 1,
	
	kInteractionList,

	kFightingTips,
	kFocus,

	kHUD,
	kPlayer_Character_State_HUD,
	kOtherPlayer_Character_State_HUD,

	kMainUI,
	kTableMenu,
};

UENUM(BlueprintType)
enum class EMenuType : uint8
{
	kAllocationSkill,
	kAllocationTalent,
	kGroupManagger,
	kRaffle,
};
