// Copyright 2019 Tefel. All Rights Reserved

#include "GravityCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerInput.h"
#include "GravitySpringArmComponent.h"

#include "GravityMovementComponent.h"

static TAutoConsoleVariable<int32> GravityCharacter(
	TEXT("Skill.DrawDebug.GravityCharacter"),
	0,
	TEXT("")
	TEXT(" default: 0"));

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

void AGravityCharacter::AddMovementInput(FVector WorldDirection, float ScaleValue /*= 1.0f*/, bool bForce /*= false*/)
{
#ifdef WITH_EDITOR
	if (GravityCharacter.GetValueOnGameThread())
	{
		//	DrawDebugLine(GetWorldImp(), OnwerActorPtr->GetActorLocation(), OnwerActorPtr->GetActorLocation() + (100 * ForwardDirection), FColor::Red, false, 3);
	}
#endif

	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
}