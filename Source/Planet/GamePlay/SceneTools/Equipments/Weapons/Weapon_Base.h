// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ToolUnitBase.h"
#include "PlanetGameplayAbility.h"
#include "Skill_Base.h"

#include "Weapon_Base.generated.h"

UCLASS()
class PLANET_API AWeapon_Base : public AToolUnitBase
{
	GENERATED_BODY()

public:

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Range")
	int32 AttackRange = 150;

};
