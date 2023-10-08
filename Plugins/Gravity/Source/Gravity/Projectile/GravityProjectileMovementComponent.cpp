
#include "GravityProjectileMovementComponent.h"

UGravityProjectileMovementComponent::UGravityProjectileMovementComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{

}

void UGravityProjectileMovementComponent::SetGravityDirection(FVector NewGravityDirection)
{
	CustomGravityDirection = NewGravityDirection.GetSafeNormal();
}

FVector UGravityProjectileMovementComponent::ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const
{
	return Super::ComputeAcceleration(InVelocity, DeltaTime);
}

bool UGravityProjectileMovementComponent::ShouldUseSubStepping() const
{
	return Super::ShouldUseSubStepping();
}	
