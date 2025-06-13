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
class PLANET_API UItemDecription_Skill_PassiveSkill : public UItemDecription
{
	GENERATED_BODY()

public:
	using FSkillProxyType = FPassiveSkillProxy;

	using FItemProxy_DescriptionType = UItemProxy_Description_PassiveSkill;

private:
	virtual void SetUIStyle() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* PropertyEntrysVerticalBox = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Title = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	URichTextBlock* DescriptionText = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPropertyEntryDescription>PropertyEntryDescriptionClass;
	
};

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
