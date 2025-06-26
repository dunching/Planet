// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffectComponent.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"

#include "GE_Component.generated.h"

UCLASS()
class NETABILITYSYSTEM_API UCancelAbilityGameplayEffectComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()

public:
	virtual bool OnActiveGameplayEffectAdded(
		FActiveGameplayEffectsContainer& ActiveGEContainer,
		FActiveGameplayEffect& ActiveGE
		) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Application)
	FGameplayTagContainer CancelAbility;
};

UCLASS()
class NETABILITYSYSTEM_API UActivationOwnedTagsGameplayEffectComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()

public:
	virtual bool OnActiveGameplayEffectAdded(
		FActiveGameplayEffectsContainer& ActiveGEContainer,
		FActiveGameplayEffect& ActiveGE
		) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Application)
	FGameplayTagContainer ActivationOwnedTags;
};
