// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Skill_Base.h"
#include "ItemProxy_Minimal.h"
#include "SceneProxyTable.h"

#include "Skill_Element_Base.generated.h"

struct FBasicProxy;

UCLASS()
class PLANET_API UItemProxy_Description_ElementtalSkill : public UItemProxy_Description_Skill
{
	GENERATED_BODY()

public:
	
};

/**
 * 元素技能
 */
UCLASS()
class USkill_Element_Base : public UPlanetGameplayAbility
{
	GENERATED_BODY()

public:

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

protected:

	TObjectPtr<ACharacterBase> CharacterPtr = nullptr;

};
