
#include "TrackTransmissionProcessor.h"

#include "Kismet/KismetMathLibrary.h"

#include "MMTBPFunctionLibrary.h"

UTrackTransmissionProcessor::UTrackTransmissionProcessor(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UTrackTransmissionProcessor::BeginPlay()
{
	Super::BeginPlay();

	GetComponentsReference();
	PreCalculateCoefficients();
}

void UTrackTransmissionProcessor::GetComponentsReference()
{
	ReferenceFrameComponentMesh = UMMTBPFunctionLibrary::GetSceneComponentReferenceByName(this, ReferenceFrameComponentName);
}

void UTrackTransmissionProcessor::PhysicsUpdate(float DeltaTime)
{
	UpdateTorque();
	UpdateAngularVelocity(DeltaTime);
}

void UTrackTransmissionProcessor::UpdateTorque()
{
	SprocketTorque = DriveTrainTorque + TrackContactFrictionTorque;
}

FVector UTrackTransmissionProcessor::GetTrackDriveForce(const FVector& TrackForce)
{
	return UKismetMathLibrary::TransformDirection(
		UMMTBPFunctionLibrary::MMTGetTransformComponent(ReferenceFrameComponentMesh, TEXT("None")),
		FVector(1, 0, 0)) * (DriveTrainTorque / SprocketRadiusCm);
}

void UTrackTransmissionProcessor::SetTrackApplicationForce(const FVector& FrictionReactionForce, float RollingFrictionForce)
{
	const auto Vec = UKismetMathLibrary::InverseTransformDirection(
		UMMTBPFunctionLibrary::MMTGetTransformComponent(ReferenceFrameComponentMesh, TEXT("None")),
		FrictionReactionForce * (TrackMass + SprocketMass)
	);

	TrackContactFrictionTorque = UKismetMathLibrary::ProjectVectorOnToVector(Vec, FVector(1, 0, 0)).X * SprocketRadiusCm;
	RollingFrictionTorque = RollingFrictionForce * SprocketRadiusCm;
}

void UTrackTransmissionProcessor::SetBrakeRatio(float NewBrakeRatio)
{
	BrakeRatio = FMath::Abs(NewBrakeRatio);
}

float UTrackTransmissionProcessor::ApplyMechanicalFriction(float NewAngularVelocity, float DeltaAngularVelocity, float DeltaTime)
{
	auto AngularVelocityIn = NewAngularVelocity;
	float AngularVelocityLoc = 0.f;

	if (
		(FMath::Abs(AngularVelocityIn) > MechanicalFrictionStaticLimit) ||
		(FMath::Abs(DeltaAngularVelocity) > MechanicalFrictionStaticLimit)
		)
	{
		AngularVelocityLoc = AngularVelocityIn - (AngularVelocityIn * MechanicalFrictionStaticLimit * DeltaTime);
	}

	return AngularVelocityLoc;
}

float UTrackTransmissionProcessor::ApplyRollingFriction(float AngularVelocityIn, float DeltaTime)
{
	float NewAngularVelocity = 0.f;
	auto Val = FMath::Abs(DeltaTime * (RollingFrictionTorque / MomentOfInertia));
	if (FMath::Abs(AngularVelocityIn) > Val)
	{
		NewAngularVelocity = AngularVelocityIn - (UKismetMathLibrary::SignOfFloat(AngularVelocityIn) * Val);
	}

	RollingFrictionTorque = 0;
	return NewAngularVelocity;
}

float UTrackTransmissionProcessor::GetTrackLinearVelocityFloat()
{
	return AngularVelocity * SprocketRadiusCm;
}

float UTrackTransmissionProcessor::GetSprocketAngularVelocity()
{
	return AngularVelocity;
}

FVector UTrackTransmissionProcessor::GetTrackLinearVelocityVector()
{
	return UKismetMathLibrary::TransformDirection(UMMTBPFunctionLibrary::MMTGetTransformComponent(ReferenceFrameComponentMesh, TEXT("None")), FVector(-1, 0, 0)) *
		AngularVelocity *
		SprocketRadiusCm;
}

void UTrackTransmissionProcessor::PreCalculateCoefficients()
{
	const auto SprocketRadiusCmTemp = FMath::Square(SprocketRadiusCm);
	MomentOfInertia = (SprocketMass * 0.5f * SprocketRadiusCmTemp) + (SprocketRadiusCmTemp * TrackMass);
}

void UTrackTransmissionProcessor::UpdateAngularVelocity(float DeltaTime)
{
	const auto Val = DeltaTime * (SprocketTorque / MomentOfInertia);

	AngularVelocity = ApplyRollingFriction(ApplyBrakes(ApplyMechanicalFriction(AngularVelocity + Val, Val, DeltaTime), DeltaTime), DeltaTime);
}

void UTrackTransmissionProcessor::SetDriveTrainTorque(float DeltaTorque)
{
	DriveTrainTorque = DeltaTorque;
}

float UTrackTransmissionProcessor::ApplyBrakes(float AngularVelocityIn, float DeltaTime)
{
	float NewAngVelocity = 0.f;
	if (FMath::IsNearlyZero(BrakeRatio))
	{
		NewAngVelocity = AngularVelocityIn;
	}
	else
	{
		const auto Val = UKismetMathLibrary::SignOfFloat(AngularVelocityIn) * DeltaTime * BrakeForce * BrakeRatio;
		if (FMath::Abs(AngularVelocityIn) > FMath::Abs(Val))
		{
			NewAngVelocity = AngularVelocityIn - Val;
		}
	}
	return NewAngVelocity;
}

void UTrackTransmissionProcessor::DebugSprocket()
{

}
