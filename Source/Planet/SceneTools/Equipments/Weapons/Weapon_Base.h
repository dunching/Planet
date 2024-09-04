// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ToolUnitBase.h"
#include "PlanetGameplayAbility.h"
#include "Skill_Base.h"

#include "Weapon_Base.generated.h"

class AHumanCharacter;
class UWeaponUnit;

UCLASS()
class PLANET_API AWeapon_Base : public AToolUnitBase
{
	GENERATED_BODY()

public:

	UWeaponUnit* WeaponUnitPtr = nullptr;

protected:

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	virtual void AttachToCharacter(ACharacterBase* CharacterPtr)override;

};
