// Copyright 2019 Tefel. All Rights Reserved

#include "GravityCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "GravityMovementComponent.h"
#include "Gravity/GravityPlanet.h"
#include "GameFramework/PlayerInput.h"

AGravityCharacter::AGravityCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UGravityMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 180.0f, 0.0f); // ...at this rotation rate

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

UGravityMovementComponent* AGravityCharacter::GetGravityMovementComponent()
{
	return Cast<UGravityMovementComponent>(GetMovementComponent());
}

FQuat AGravityCharacter::GetGravityToWorldTransform() const
{
    FQuat GravityTransform = FQuat::Identity;
    const UCharacterMovementComponent* const MovementComponent = GetCharacterMovement();
    if (MovementComponent)
    {
        GravityTransform = MovementComponent->GetGravityToWorldTransform();
    }

    return GravityTransform;
}