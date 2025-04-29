// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GravityMovementComponent.h"

#include "PlanetCharacterMovementComponent.generated.h"

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UPlanetCharacterMovementComponent : public UGravityMovementComponent
{
    GENERATED_BODY()

public:

    virtual void PhysicsRotation(float DeltaTime)override;
    
    virtual void OnRootMotionSourceBeingApplied(const FRootMotionSource* Source)override;
};
