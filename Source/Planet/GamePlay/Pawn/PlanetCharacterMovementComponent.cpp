
#include "PlanetCharacterMovementComponent.h"

#include "GameFramework/Character.h"

#include "CharacterBase.h"
#include "KismetGravityLibrary.h"
#include "StateProcessorComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UPlanetCharacterMovementComponent::PhysicsRotation(
	float DeltaTime
)
{	if (bForceRotation)
	{
	}
	else if (
//		HasRootMotionSources() ||
		HasAnimRootMotion()
		)
	{
		return;
	}

	// 
	if (!(bOrientRotationToMovement || bUseControllerDesiredRotation))
	{
		return;
	}

	if (!HasValidData() || (!CharacterOwner->Controller && !bRunPhysicsWithNoController))
	{
		return;
	}

	FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
	CurrentRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): CurrentRotation"));

	FRotator DeltaRot = GetDeltaRotation(DeltaTime);
	DeltaRot.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): GetDeltaRotation"));

	auto FocusCharactersAry = Cast<ACharacterBase>(CharacterOwner)->GetStateProcessorComponent()->GetTargetCharactersAry();

	FRotator DesiredRotation = CurrentRotation;
	if (bForceRotation)
	{
		DesiredRotation = ComputeRootMotionToMovementRotation(CurrentRotation, DeltaTime, DeltaRot);
	}
	else if (FocusCharactersAry.IsValidIndex(0) && FocusCharactersAry[0].IsValid())
	{
		const auto CurrentLocation = UpdatedComponent->GetComponentLocation(); // Normalized
		const auto Z = -UKismetGravityLibrary::GetGravity();
		DesiredRotation = UKismetMathLibrary::MakeRotFromZX(
		Z, FocusCharactersAry[0]->GetActorLocation() - CurrentLocation
		);
	}
	else if (bOrientRotationToMovement)
	{
		DesiredRotation = ComputeOrientToMovementRotation(CurrentRotation, DeltaTime, DeltaRot);
	}
	else if (CharacterOwner->Controller && bUseControllerDesiredRotation)
	{
		if (
			bSkip_Rotation
			)
		{
			return;
		}
		DesiredRotation = CharacterOwner->Controller->GetDesiredRotation();
	}
	else if (!CharacterOwner->Controller && bRunPhysicsWithNoController && bUseControllerDesiredRotation)
	{
		if (
			bSkip_Rotation
			)
		{
			return;
		}
		if (AController* ControllerOwner = Cast<AController>(CharacterOwner->GetOwner()))
		{
			DesiredRotation = ControllerOwner->GetDesiredRotation();
		}
	}
	else
	{
		return;
	}

	const bool bWantsToBeVertical = ShouldRemainVertical();

	if (bWantsToBeVertical)
	{
		if (HasCustomGravity())
		{
			FRotator GravityRelativeDesiredRotation = (GetGravityToWorldTransform() * DesiredRotation.Quaternion()).Rotator();
			GravityRelativeDesiredRotation.Pitch = 0.f;
			GravityRelativeDesiredRotation.Yaw = FRotator::NormalizeAxis(GravityRelativeDesiredRotation.Yaw);
			GravityRelativeDesiredRotation.Roll = 0.f;
			DesiredRotation = (GetWorldToGravityTransform() * GravityRelativeDesiredRotation.Quaternion()).Rotator();
		}
		else
		{
			DesiredRotation.Pitch = 0.f;
			DesiredRotation.Yaw = FRotator::NormalizeAxis(DesiredRotation.Yaw);
			DesiredRotation.Roll = 0.f;
		}
	}
	else
	{
		DesiredRotation.Normalize();
	}

	// Accumulate a desired new rotation.
	const float AngleTolerance = 1e-3f;

	if (!CurrentRotation.Equals(DesiredRotation, AngleTolerance))
	{
		// If we'd be prevented from becoming vertical, override the non-yaw rotation rates to allow the character to snap upright
		if (true && bWantsToBeVertical)
		{
			if (FMath::IsNearlyZero(DeltaRot.Pitch))
			{
				DeltaRot.Pitch = 360.0;
			}
			if (FMath::IsNearlyZero(DeltaRot.Roll))
			{
				DeltaRot.Roll = 360.0;
			}
		}

		if (HasCustomGravity())
		{
			FRotator GravityRelativeCurrentRotation = (GetGravityToWorldTransform() * CurrentRotation.Quaternion()).Rotator();
			FRotator GravityRelativeDesiredRotation = (GetGravityToWorldTransform() * DesiredRotation.Quaternion()).Rotator();

			// PITCH
			if (!FMath::IsNearlyEqual(GravityRelativeCurrentRotation.Pitch, GravityRelativeDesiredRotation.Pitch, AngleTolerance))
			{
				GravityRelativeDesiredRotation.Pitch = FMath::FixedTurn(GravityRelativeCurrentRotation.Pitch, GravityRelativeDesiredRotation.Pitch, DeltaRot.Pitch);
			}

			// YAW
			if (!FMath::IsNearlyEqual(GravityRelativeCurrentRotation.Yaw, GravityRelativeDesiredRotation.Yaw, AngleTolerance))
			{
				GravityRelativeDesiredRotation.Yaw = FMath::FixedTurn(GravityRelativeCurrentRotation.Yaw, GravityRelativeDesiredRotation.Yaw, DeltaRot.Yaw);
			}

			// ROLL
			if (!FMath::IsNearlyEqual(GravityRelativeCurrentRotation.Roll, GravityRelativeDesiredRotation.Roll, AngleTolerance))
			{
				GravityRelativeDesiredRotation.Roll = FMath::FixedTurn(GravityRelativeCurrentRotation.Roll, GravityRelativeDesiredRotation.Roll, DeltaRot.Roll);
			}

			DesiredRotation = (GetWorldToGravityTransform() * GravityRelativeDesiredRotation.Quaternion()).Rotator();
		}
		else
		{
			// PITCH
			if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance))
			{
				DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
			}

			// YAW
			if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
			{
				DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
			}

			// ROLL
			if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
			{
				DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
			}
		}

		// Set the new rotation.
		DesiredRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));
		MoveUpdatedComponent(FVector::ZeroVector, DesiredRotation, /*bSweep*/ false);
	}
}

void UPlanetCharacterMovementComponent::OnRootMotionSourceBeingApplied(
	const FRootMotionSource* Source
)
{
	Super::OnRootMotionSourceBeingApplied(Source);
}
