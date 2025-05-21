// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayCameraCommon.generated.h"

UENUM(BlueprintType, Blueprintable)
enum class ECameraType : uint8
{
	kAction,
	kDashing,
	kRunning,
	kReleasing_ActiveSkill_XYFH,
};
