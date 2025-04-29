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
	public UAbilityTask_ApplyRootMotion_Base
{
	GENERATED_BODY()

public:
	UAbilityTask_ARM_RadialForce(
		const FObjectInitializer& ObjectInitializer
	);

	static UAbilityTask_ARM_RadialForce* MyApplyRootMotionRadialForce(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		TWeakObjectPtr<ATractionPoint> TractionPoinAcotrPtr,
		float Strength,
		float Duration,
		float Radius,
		float InnerRadius,
		bool bIsPush
	);

	void UpdateLocation(
		TWeakObjectPtr<ATractionPoint> TractionPointPtr
	);

protected:
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	virtual void SharedInitAndApply() override;

	virtual void Activate() override;

	virtual void TickTask(
		float DeltaTime
	) override;

	virtual void OnDestroy(
		bool AbilityIsEnding
	) override;

	void UpdateTarget();

	FOnTaskFinished OnFinish;

	const float Interval = .1f;

	float IntervalTime = 0.f;

	UPROPERTY(Replicated)
	TWeakObjectPtr<ATractionPoint> TractionPoinAcotrPtr = nullptr;

	UPROPERTY(Replicated)
	float Strength = 100.f;

	UPROPERTY(Replicated)
	float Duration = 1.f;

	UPROPERTY(Replicated)
	float Radius = 200.f;

	UPROPERTY(Replicated)
	float InnerRadius = 100.f;

	UPROPERTY(Replicated)
	bool bIsPush = false;

	float CurrentTime = 0.f;
};
