// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "Abilities/Tasks/AbilityTask_ApplyRootMotionRadialForce.h"

#include "AbilityTask_MyApplyRootMotionRadialForce.generated.h"

DECLARE_DELEGATE(FOnTaskFinished);

/**
 *	Applies force to character's movement
 */
UCLASS()
class PLANET_API UAbilityTask_MyApplyRootMotionRadialForce :
	public UAbilityTask_ApplyRootMotionRadialForce
{
	GENERATED_UCLASS_BODY()	
	
	static UAbilityTask_MyApplyRootMotionRadialForce* MyApplyRootMotionRadialForce(
		UGameplayAbility* OwningAbility, 
		FName TaskInstanceName, 
		FVector Location, 
		AActor* LocationActor,
		float Strength, 
		float Duration, 
		float Radius,
		bool bIsPush,
		bool bIsAdditive,
		bool bNoZForce,
		UCurveFloat* StrengthDistanceFalloff,
		UCurveFloat* StrengthOverTime,
		bool bUseFixedWorldDirection, 
		FRotator FixedWorldDirection, 
		ERootMotionFinishVelocityMode VelocityOnFinishMode, 
		FVector SetVelocityOnFinish, 
		float ClampVelocityOnFinish
	);

	virtual void TickTask(float DeltaTime) override;

	virtual void OnDestroy(bool AbilityIsEnding) override;

	void UpdateLocation(const FVector &Location);

	FOnTaskFinished OnFinish;

};
