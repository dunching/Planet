// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "Abilities/Tasks/AbilityTask_ApplyRootMotionRadialForce.h"

#include "AbilityTask_ARM_RadialForce.generated.h"

DECLARE_DELEGATE(FOnTaskFinished);

class ATractionPoint;

/**
 *	Applies force to character's movement
 */
UCLASS()
class PLANET_API UAbilityTask_ARM_RadialForce :
	public UAbilityTask_ApplyRootMotionRadialForce
{
	GENERATED_UCLASS_BODY()	

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	static UAbilityTask_ARM_RadialForce* MyApplyRootMotionRadialForce(
		UGameplayAbility* OwningAbility, 
		FName TaskInstanceName, 
		TWeakObjectPtr<ATractionPoint>TractionPointPtr
	);

	virtual void SharedInitAndApply() override;

	virtual void TickTask(float DeltaTime) override;

	virtual void OnDestroy(bool AbilityIsEnding) override;

	void UpdateLocation(TWeakObjectPtr<ATractionPoint>TractionPointPtr);

	FOnTaskFinished OnFinish;
	
};
