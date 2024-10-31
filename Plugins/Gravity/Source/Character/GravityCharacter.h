// Copyright 2019 Tefel. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GravityCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

class UGravityMovementComponent;
class AGravityPlanet;

// Gravity character class which overrides gravity movement component
UCLASS()
class GRAVITY_API AGravityCharacter : public ACharacter
{
    GENERATED_BODY()

public:

    // Sets default values for this character's properties
    AGravityCharacter(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
    UGravityMovementComponent* GetGravityMovementComponent();

    FQuat GetGravityToWorldTransform() const;

    virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.0f, bool bForce = false)override;

protected:

};