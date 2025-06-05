// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Skill_Element_Base.h"
#include "GenerateTypes.h"
#include "SceneProxyTable.h"

#include "Skill_Element_Metal.generated.h"

class UAbilityTask_TimerHelper;

struct FBasicProxy;

UCLASS()
class PLANET_API UItemProxy_Description_ElementtalSkill_Metal : public UItemProxy_Description_ElementtalSkill
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPerLevelValue_Float PerLevel_CriticalHitRate = {
		0.5f, 0.5f, 0.5f,
		1.0f, 1.0f, 1.0f,
		1.5f, 1.5f, 1.5f,
	};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPerLevelValue_Float PerLevel_CriticalDamage = {
		1.5f, 1.5f, 1.5f, 1.5f,
		2.0f, 2.0f, 2.0f, 2.0f,
		2.5f, 2.5f, 2.5f, 2.5f,
	};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPerLevelValue_Float PerLevel_PercentPenetration = {
		0.f, 0.f, 0.f,
		1.0f, 1.0f, 1.0f,
		1.5f, 1.5f, 1.5f,
	};
};

UCLASS()
class USkill_Element_Metal : public USkill_Element_Base
{
	GENERATED_BODY()

public:
	using FItemProxy_DescriptionType = UItemProxy_Description_ElementtalSkill_Metal;

	using FValueChangedDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
		) override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UItemProxy_Description_ElementtalSkill_Metal> ItemProxy_Description_Ref;

private:
	void OnValueChanged(
		const FOnAttributeChangeData& CurrentValue
		);

	TObjectPtr<FItemProxy_DescriptionType> ItemProxy_DescriptionPtr = nullptr;
};
