// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemDecription.h"

#include "Skill_Base.h"
#include "ItemProxy_Minimal.h"

#include "Skill_Passive_Base.generated.h"

class UVerticalBox;
class UTextBlock;
class URichTextBlock;

class UPropertyEntryDescription;

struct FBasicProxy;

UCLASS()
class USkill_Passive_Base : public USkill_Base
{
	GENERATED_BODY()

public:
	USkill_Passive_Base();

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
		) override;

protected:
	virtual void OnRemoveAbility(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
		) override;
};
