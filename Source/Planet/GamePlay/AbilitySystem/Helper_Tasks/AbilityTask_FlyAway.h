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

	virtual void Activate() override;

	virtual void OnDestroy(bool AbilityIsEnding) override;

	void UpdateDuration();

	FOnTaskFinished OnFinish;

protected:

	virtual void SharedInitAndApply() override;

protected:

	int32 Height = 300;

	float Duration = 2.f;

	ERootMotionAccumulateMode RootMotionAccumulateMode = ERootMotionAccumulateMode::Additive;

};
