// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/ProjectileMovementComponent.h"

#include "GravityProjectileMovementComponent.generated.h"

UCLASS()
class GRAVITY_API UGravityProjectileMovementComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()

public:

	UGravityProjectileMovementComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(Category = "Pawn|Components|CharacterMovement", BlueprintCallable)
		virtual void SetGravityDirection(FVector NewGravityDirection);

protected:

	/** Compute the acceleration that will be applied */
	virtual FVector ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const override;

	/**
	 * Determine whether or not to use substepping in the projectile motion update.
	 * If true, GetSimulationTimeStep() will be used to time-slice the update. If false, all remaining time will be used during the tick.
	 * @return Whether or not to use substepping in the projectile motion update.
	 * @see GetSimulationTimeStep()
	 */
	virtual bool ShouldUseSubStepping() const override;

	UPROPERTY()
		FVector CustomGravityDirection = FVector::ZeroVector;

};