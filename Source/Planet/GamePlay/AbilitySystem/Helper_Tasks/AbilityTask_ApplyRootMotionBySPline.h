// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotion_Base.h"

#include "AbilityTask_ApplyRootMotionBySPline.generated.h"

class UCurveFloat;
class ASPlineActor;
class ACharacterBase;

DECLARE_DELEGATE(FOnTaskFinished);

/**
 *	Applies force to character's movement
 */
UCLASS()
class PLANET_API UAbilityTask_ApplyRootMotionBySPline : public UAbilityTask_ApplyRootMotion_Base
{
	GENERATED_BODY()
public:

	FOnTaskFinished OnFinish;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_ApplyRootMotionBySPline* ApplyRootMotionBySpline
	(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		float Duration,
		ASPlineActor* InSPlineActorPtr,
		ACharacterBase* InTargetCharacterPtr
	);

	virtual void Activate() override;

	virtual void TickTask(float DeltaTime) override;

	virtual void OnDestroy(bool AbilityIsEnding) override;

protected:

	virtual void SharedInitAndApply() override;

protected:

	float Duration;

	ASPlineActor* SPlineActorPtr = nullptr;

	ACharacterBase* TargetCharacterPtr = nullptr;

};