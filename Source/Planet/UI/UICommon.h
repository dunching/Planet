// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum EUIOrder : int8
{
	// WidgetComponent默认为-100
	kUIOrderDefault = -99,

	kConversationBorder = 1,
	
	kInteractionList,

	kFightingTips,
	
	kFocus,

	kHUD,
	kOtherWidget,

	/**
	 * 玩家的角色的Title
	 */
	kPlayer_Character_State_Title,
	
	/**
	 * NPC的角色的Title
	 */
	kOtherPlayer_Character_State_Title,

	kMainUI,
	
	kTableMenu,

	kHoverDecription,
};
