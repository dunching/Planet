// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Skill_Base.h"
#include "SceneElement.h"

#include "Skill_Consumable_Base.generated.h"

class AConsumable_Base;

UCLASS()
class USkill_Consumable_Base : public USkill_Base
{
	GENERATED_BODY()

public:

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

	virtual bool CanUse()const;

protected:
	
};
