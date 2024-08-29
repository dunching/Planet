// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"

#include "ToolUnitBase.h"
#include "PlanetGameplayAbility.h"
#include "Skill_Base.h"

#include "Tool_Base.generated.h"

class UToolFuture_Base;

UCLASS()
class PLANET_API ATool_Base : public AToolUnitBase
{
	GENERATED_BODY()

public:

	virtual void AttachToCharacter(ACharacterBase* CharacterPtr)override;

	virtual void DoActionByCharacter(AHumanCharacter* CharacterPtr, EEquipmentActionType ActionType);

protected:

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TSubclassOf<UToolFuture_Base> EquipmentAbilities;

	FGameplayAbilitySpecHandle EquipmentAbilitieHandle;

};
