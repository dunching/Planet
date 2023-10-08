
#include "4WheeledVehicleBase.h"

#include "DrawDebugHelpers.h"

#include "GravityPlanet.h"

A4WheeledVehicleBase::A4WheeledVehicleBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void A4WheeledVehicleBase::BeginPlay()
{
	Super::BeginPlay();

 	GetWorld()->GetTimerManager().SetTimer(
 		UpdateGravityCenterLocationTimer, this, &A4WheeledVehicleBase::UpdatePlanetGravity, 0.1f, true
 	);
}

void A4WheeledVehicleBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void A4WheeledVehicleBase::UpdatePlanetGravity()
 {
 	HasGravity = false;
 
 	const auto ActorLocation = GetActorLocation();
 
 	TArray<AActor*>PlanetAry;
 	GetOverlappingActors(PlanetAry, AGravityPlanet::StaticClass());
 
 	if (PlanetAry.Num() > 0)
 	{
 		HasGravity = true;
 		auto NearestPlanetPtr = PlanetAry[0];
 
 		auto Distance = FVector::Distance(NearestPlanetPtr->GetActorLocation(), ActorLocation);
 
 		for (int32 Index = 1; Index < PlanetAry.Num(); Index++)
 		{
 			const auto NewInstance = FVector::Distance(NearestPlanetPtr->GetActorLocation(), ActorLocation);
 			if (NewInstance < Distance)
 			{
 				NearestPlanetPtr = PlanetAry[Index];
 				Distance = NewInstance;
 			}
 		}
 
 		auto NewDir = GetGravityDirectionInterp(0.1f);
 
 		SetGravityDirection(NewDir);
 	}
 }

 FVector A4WheeledVehicleBase::GetGravityDirectionInterp(float DeltaSeconds)
 {
 	const auto Dir = GetGravityDirection();
 
 	const auto Location = GetActorLocation();
 	const auto NewDir = (CenterLocation - Location).GetSafeNormal();
 
 	return FMath::VInterpTo(Dir, NewDir, DeltaSeconds, 5.f);
 }

 FVector A4WheeledVehicleBase::GetGravityDirection(bool bAvoidZeroGravity /*= false*/) const
 {
 	return CustomGravityDirection;
 }
 
 void A4WheeledVehicleBase::SetGravityDirection(FVector NewGravityDirection)
 {
 	CustomGravityDirection = NewGravityDirection;
 }

void A4WheeledVehicleBase::OnMoveForward_Implementation(float Value)
{

}

void A4WheeledVehicleBase::OnMoveRight_Implementation(float Value)
{

}

void A4WheeledVehicleBase::CustomPhysics(float DeltaTime, FBodyInstance* BodyInstance)
{
//	Super::CustomPhysics(DeltaTime, BodyInstance);
// 
	BodyInstance->ClearTorques();
	BodyInstance->ClearForces();
	const auto ForceVal = CustomGravityDirection * BodyInstance->GetMassOverride();
	BodyInstance->AddForce(ForceVal, true, true);

	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + CustomGravityDirection * 100, FColor::Yellow, false, 1);

	MMTPawnTransform = MMTGetTransformThisPawn();
	MMTPhysicsTick(DeltaTime);
//
////	BodyInstance->AddForce(FVector(0, 0, -1500), true, true);

}

USkeletalMeshComponent* A4WheeledVehicleBase::GetMesh()
{
	return nullptr;
}
