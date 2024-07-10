// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"

#include "ToolUnitBase.h"
#include "PlanetGameplayAbility.h"
#include "Skill_Base.h"

#include "Consumable_Base.generated.h"

class USkill_Consumable_Base;

UCLASS()
class PLANET_API AConsumable_Base : public AToolUnitBase
{
	GENERATED_BODY()

public:

	virtual void Interaction(ACharacterBase* CharacterPtr)override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TSubclassOf<USkill_Consumable_Base> Skill_Consumables_Class;

};
