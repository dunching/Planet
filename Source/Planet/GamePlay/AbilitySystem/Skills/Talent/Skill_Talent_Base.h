// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Skill_Base.h"
#include "SceneElement.h"

#include "Skill_Talent_Base.generated.h"

class UBasicUnit;

// 是否应该为 UObject ？
class FTalent_Base
{
public:

	virtual ~FTalent_Base();

protected:

};

UCLASS()
class USkill_Talent_Base : public USkill_Base
{
	GENERATED_BODY()

public:

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

protected:

};
