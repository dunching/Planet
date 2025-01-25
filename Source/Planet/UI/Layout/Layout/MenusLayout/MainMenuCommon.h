// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MainMenuCommon.generated.h"

UENUM(BlueprintType)
enum class EMenuType : uint8
{
	kAllocationSkill,
	kAllocationTalent,
	kGroupManagger,
	kRaffle,
};
