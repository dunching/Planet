// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "TeamMates_GenericType.generated.h"

UENUM(BlueprintType)
enum class ETeammateOption : uint8
{
	// 自由状态
	kFree,

	// 作为队友时的状态
	kFollow,			// 跟随 
	kAssistance,		// 协助
	kFireTarget,		// 集火目标

	// 具有攻击性的敌人
	kEnemy,

	// Test
	kTest,
	
	// 中立
	kNeutrality,

	// 初始状态
	kInitialize,

	// 说话状态
	kConversation,

	kNone,

};
