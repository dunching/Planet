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

    UCameraComponent* GetCameraComp() {
        return FollowCamera;
    };

protected:

    /** Camera boom positioning the camera behind the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    /** Follow camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;

};