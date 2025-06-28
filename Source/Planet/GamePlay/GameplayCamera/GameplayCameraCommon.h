// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayCameraCommon.generated.h"

UENUM(BlueprintType, Blueprintable)
enum class ECameraType : uint8
{
	kAction,

	// 振动的参数传入后不生效，暂时通过添加节点的方式处理
	kAction_WithShake,
	
	kDashing,
	kRunning,
	kReleasing_ActiveSkill_XYFH,
};
