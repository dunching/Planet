
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GuideThreadType.generated.h"


UENUM()
enum class EGuideThreadType : uint8
{
	kMain,
	kBrand,
	kChallenge,
	kArea,
};
