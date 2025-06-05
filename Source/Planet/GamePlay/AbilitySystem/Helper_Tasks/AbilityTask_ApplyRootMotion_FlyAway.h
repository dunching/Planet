// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotion_Base.h"
#include "GameFramework/RootMotionSource.h"

#include "AbilityTask_ApplyRootMotion_FlyAway.generated.h"

class UGameplayAbility;
class ATornado;
class ACharacterBase;

DECLARE_DELEGATE(FOnTaskFinished);

/**
 * 使角色进入飞行状态
 */
UCLASS()
class PLANET_API UAbilityTask_ApplyRootMotion_FlyAway : public UAbilityTask_ApplyRootMotion_Base
{
	GENERATED_BODY()

public:
	UAbilityTask_ApplyRootMotion_FlyAway(
		const FObjectInitializer& ObjectInitializer
	);

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	static UAbilityTask_ApplyRootMotion_FlyAway* NewTask(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		float Duration,
		float Height
	);

	static UAbilityTask_ApplyRootMotion_FlyAway* NewTask(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		ERootMotionAccumulateMode RootMotionAccumulateMode,
		float Duration,
		float Height
	);

	static UAbilityTask_ApplyRootMotion_FlyAway* NewTask(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		ERootMotionAccumulateMode RootMotionAccumulateMode,
		float Duration,
		float Height,
		int32 ResingSpeed,
		int32 FallingSpeed
	);

	virtual void TickTask(
		float DeltaTime
	) override;

	virtual void Activate() override;

	virtual void OnDestroy(
		bool AbilityIsEnding
	) override;

	void UpdateDuration(
		int32 InHeight,
		float InDuration
	);

	void UpdateDuration();

	FOnTaskFinished OnFinished;

protected:
	virtual void SharedInitAndApply() override;

protected:
	UPROPERTY(Replicated)
	int32 Height = 100;

	UPROPERTY(Replicated)
	float Duration = 1.f;

	UPROPERTY(Replicated)
	int32 ResingSpeed = 100;

	UPROPERTY(Replicated)
	int32 FallingSpeed = 100;

	UPROPERTY(Replicated)
	ERootMotionAccumulateMode RootMotionAccumulateMode = ERootMotionAccumulateMode::Additive;
};

/**
 * 使角色进入被击飞状态
 * 类似进行一次指定高度的跳跃行为
 */
UCLASS()
class PLANET_API UAbilityTask_HasBeenFlyAway : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAbilityTask_HasBeenFlyAway(
		const FObjectInitializer& ObjectInitializer
	);

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	static UAbilityTask_HasBeenFlyAway* NewTask(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		int32 Height
	);

	virtual void Activate() override;

	virtual void TickTask(
		float DeltaTime
	) override;

	FOnTaskFinished OnFinished;

protected:
	UPROPERTY(Replicated)
	int32 Height = 300;

	UPROPERTY(Replicated)
	int32 CurrentDistance = 0;

	UPROPERTY(Replicated)
	bool bHasBeenApex = false;

	int32 Line = 10000;
};
