// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotion_Base.h"

#include "AbilityTask_ARM_MoveToForce.generated.h"

class UCurveFloat;

DECLARE_DELEGATE(FOnTaskFinished);

/**
 *	Applies force to character's movement
 */
UCLASS()
class PLANET_API UAbilityTask_ARM_MoveToForce : public UAbilityTask_ApplyRootMotion_Base
{
	GENERATED_UCLASS_BODY()
public:

	FOnTaskFinished OnFinished;

	/** Apply force to character's movement */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_ARM_MoveToForce* ApplyRootMotionMoveToForce
	(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		FVector StartLocation,
		FVector TargetLocation,
		float Duration
	);

	virtual void Activate() override;

	/** Tick function for this task, if bTickingTask == true */
	virtual void TickTask(float DeltaTime) override;

	virtual void PreDestroyFromReplication() override;

	virtual void OnDestroy(bool AbilityIsEnding) override;

protected:

	virtual void SharedInitAndApply() override;

protected:
	
	UPROPERTY(Replicated)
	FVector StartLocation;
	
	UPROPERTY(Replicated)
	FVector TargetLocation;
	
	UPROPERTY(Replicated)
	float Duration;

};
