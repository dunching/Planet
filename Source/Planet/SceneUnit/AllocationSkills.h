// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "GenerateType.h"
#include "BaseData.h"
#include "GAEvent_Helper.h"

#include "AllocationSkills.generated.h"

class USkillUnit;
class UWeaponUnit;

struct FSkillSocket
{
	FGameplayTag SkillSocket;

	USkillUnit* SkillUnit = nullptr;
};

struct FWeaponSocket
{
	FGameplayTag SkillSocket;

	UWeaponUnit* WeaponUnitPtr = nullptr;
};

USTRUCT(BlueprintType)
struct PLANET_API FAllocationSkills final
{
	GENERATED_USTRUCT_BODY()

	TMap<FGameplayTag, TSharedPtr<FSkillSocket>>SkillsMap;
	
	TMap<FGameplayTag, TSharedPtr<FWeaponSocket>>WeaponsMap;

};