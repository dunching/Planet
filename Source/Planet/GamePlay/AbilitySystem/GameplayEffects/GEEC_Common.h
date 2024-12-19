// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExecutionCalculation.h"

#include "GEEC_Common.generated.h"

UCLASS()
class PLANET_API UGEEC_Reply : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	) const;

protected:
	
};

UCLASS()
class PLANET_API UGEEC_Running : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	) const;

protected:
	
};
