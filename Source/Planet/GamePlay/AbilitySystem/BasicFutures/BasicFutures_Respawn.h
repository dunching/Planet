// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BasicFuturesBase.h"
#include "GenerateTypes.h"

#include "BasicFutures_Respawn.generated.h"

class UAnimMontage;

class ACharacterBase;
class ATeleport;

UCLASS()
class PLANET_API UBasicFutures_Respawn : public UBasicFuturesBase
{
	GENERATED_BODY()

public:

	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

protected:

	// virtual void InitalDefaultTags()override;

	void PlayMontage(UAnimMontage* CurMontagePtr, float Rate);

	void OnMontageComplete();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* DeathMontage = nullptr;

	ACharacterBase* CharacterPtr = nullptr;
};