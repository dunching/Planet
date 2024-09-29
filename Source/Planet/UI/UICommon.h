// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UICommon.generated.h"

enum EUIOrder :uint8
{
	// 
	kPlayer_Character_State_HUD,
	kCharacter_State_HUD,

	kFightingTips,
	kFocus,
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
