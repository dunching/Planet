// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Skill_Base.h"
#include "SceneElement.h"

#include "Skill_Active_Base.generated.h"

class UBasicUnit;

UCLASS()
class USkill_Active_Base : public USkill_Base
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Require Weapon")
	EWeaponUnitType WeaponUnitType = EWeaponUnitType::kNone;

protected:

};
