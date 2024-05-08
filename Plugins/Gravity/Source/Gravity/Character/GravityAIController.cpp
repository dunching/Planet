
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
		FRotator NewControlRotation = ControlRotationWithoutGravityTrans;

		// Look toward focus
		const FVector FocalPoint = GetFocalPoint();
		if (FAISystem::IsValidLocation(FocalPoint))
		{
			NewControlRotation = (FocalPoint - MyPawn->GetPawnViewLocation()).Rotation();
		}
		else if (bSetControlRotationFromPawnOrientation)
		{
			NewControlRotation = ControlRotationWithoutGravityTrans;
		}

		// Don't pitch view unless looking at another pawn
		if (NewControlRotation.Pitch != 0 && Cast<APawn>(GetFocusActor()) == nullptr)
		{
			NewControlRotation.Pitch = 0.f;
		}

		ControlRotationWithoutGravityTrans = NewControlRotation;

		NewControlRotation = (MyPawn->GetGravityTransform() * NewControlRotation.Quaternion()).Rotator();

		SetControlRotation(NewControlRotation);

		if (bUpdatePawn)
		{
			const FRotator CurrentPawnRotation = MyPawn->GetActorRotation();

			if (CurrentPawnRotation.Equals(NewControlRotation, 1e-3f) == false)
			{
				MyPawn->FaceRotation(NewControlRotation, DeltaTime);
			}
		}
	}
}
