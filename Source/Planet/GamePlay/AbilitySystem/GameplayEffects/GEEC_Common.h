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

class ACharacterBase;

UCLASS()
class PLANET_API UGEEC_Base : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
protected:

	void ApplyModifyData(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
		const TMap<FGameplayTag, float>& SetByCallerTagMagnitudes,
		TObjectPtr<ACharacterBase>InstigatorPtr,
		TObjectPtr<ACharacterBase>TargetCharacterPtr
		)const;
	
};

/**
 * 通用的数据流转
 * 在自定义操作里面做一些输入输出的修正
 */
UCLASS()
class PLANET_API UGEEC_DataModify : public UGEEC_Base
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
class PLANET_API UGEEC_Reply : public UGEEC_Base
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

UCLASS()
class PLANET_API UGEEC_CancelRunning : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	) const;

protected:
	
};
