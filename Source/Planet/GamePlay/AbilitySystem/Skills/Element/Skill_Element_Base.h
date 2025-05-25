// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Skill_Base.h"
#include "ItemProxy_Minimal.h"

#include "Skill_Element_Base.generated.h"

struct FBasicProxy;

/**
 * 元素技能
 */
UCLASS()
class USkill_Element_Base : public USkill_Base
{
	GENERATED_BODY()

public:

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

protected:

};
