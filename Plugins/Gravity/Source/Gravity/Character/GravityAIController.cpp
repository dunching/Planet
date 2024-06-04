
#include "GravityAIController.h"

#include <Engine/Engine.h>
#include <GameFramework/Character.h>
#include <IXRTrackingSystem.h>
#include <IXRCamera.h>
#include <Kismet/KismetMathLibrary.h>

#include "GravityCharacter.h"
#include "GravityMovementComponent.h"

void AGravityAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		FRotator NewControlRotation = GetControlRotation();

		// Look toward focus
		const FVector FocalPoint = GetFocalPoint();
		if (FAISystem::IsValidLocation(FocalPoint))
		{
			NewControlRotation = (FocalPoint - MyPawn->GetPawnViewLocation()).Rotation();
		}
		else if (bSetControlRotationFromPawnOrientation)
		{
			NewControlRotation = MyPawn->GetActorRotation();
		}

		NewControlRotation.Pitch = 0.f;
		NewControlRotation.Roll = 0.f;

		SetControlRotation(NewControlRotation);

		if (bUpdatePawn)
		{
			const FRotator CurrentPawnRotation = MyPawn->GetActorRotation();

			if (CurrentPawnRotation.Equals(NewControlRotation, 1e-3f) == false)
			{
#if WITH_EDITOR
				DrawDebugLine(
					GetWorld(), 
					MyPawn->GetActorLocation(),
					MyPawn->GetActorLocation() + (NewControlRotation.Vector() * 300),
					FColor::Yellow,
					false,
					3.f
				);

				RootComponent->SetWorldLocation(MyPawn->GetActorLocation());
#endif

				MyPawn->FaceRotation(NewControlRotation, DeltaTime);
			}
		}
	}
}
