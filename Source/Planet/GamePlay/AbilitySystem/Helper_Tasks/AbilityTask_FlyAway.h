// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotion_Base.h"
#include "GameFramework/RootMotionSource.h"

#include "AbilityTask_FlyAway.generated.h"

class UGameplayAbility;
class ATornado;
class ACharacterBase;

DECLARE_DELEGATE(FOnTaskFinished);

UCLASS()
class PLANET_API UAbilityTask_FlyAway : public UAbilityTask_ApplyRootMotion_Base
{
	GENERATED_BODY()

public:

	UAbilityTask_FlyAway(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	static UAbilityTask_FlyAway* NewTask(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		float Duration,
		float Height
	);
	
	static UAbilityTask_FlyAway* NewTask(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		ERootMotionAccumulateMode RootMotionAccumulateMode,
		float Duration,
		float Height
	);

	virtual void TickTask(float DeltaTime) override;

	virtual void Activate() override;

	virtual void OnDestroy(bool AbilityIsEnding) override;

	void UpdateDuration(int32 InHeight, float InDuration);
	
	void UpdateDuration();

	FOnTaskFinished OnFinish;

protected:

	virtual void SharedInitAndApply() override;

protected:
	
	UPROPERTY(Replicated)
	int32 Height = 100;
	
	UPROPERTY(Replicated)
	float Duration = 1.f;
	
	UPROPERTY(Replicated)
	ERootMotionAccumulateMode RootMotionAccumulateMode = ERootMotionAccumulateMode::Additive;

};
