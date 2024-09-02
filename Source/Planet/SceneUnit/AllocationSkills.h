// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "GenerateType.h"
#include "BaseData.h"
#include "GAEvent_Helper.h"

#include "AllocationSkills.generated.h"

struct FSkillProxy;
struct FWeaponProxy;

struct FSkillSocket
{
	FGameplayTag SkillSocket;

	FSkillProxy* SkillUnit = nullptr;
};

struct FWeaponSocket
{
	FGameplayTag SkillSocket;

	FWeaponProxy* WeaponUnitPtr = nullptr;
};

USTRUCT(BlueprintType)
struct PLANET_API FAllocationSkills final
{
	GENERATED_USTRUCT_BODY()

	TMap<FGameplayTag, TSharedPtr<FSkillSocket>>SkillsMap;
	
	TMap<FGameplayTag, TSharedPtr<FWeaponSocket>>WeaponsMap;

};