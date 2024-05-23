#include "ProjectileBase.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CollisionDataStruct.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include <DrawDebugHelpers.h>
#include "Components/SphereComponent.h"

AProjectileBase::AProjectileBase(const FObjectInitializer& ObjectInitializer) :
    Super(ObjectInitializer)
{
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = CollisionComp;

    // Use a sphere as a simple collision representation
    StaticMeshCompPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
    StaticMeshCompPtr->SetSimulatePhysics(false);
    StaticMeshCompPtr->SetGenerateOverlapEvents(true);
    StaticMeshCompPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    StaticMeshCompPtr->SetMobility(EComponentMobility::Movable);
    StaticMeshCompPtr->SetupAttachment(CollisionComp);

    // Use a ProjectileMovementComponent to govern this projectile's movement
    ProjectileMovementCompPtr = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovementCompPtr->UpdatedComponent = CollisionComp;
    ProjectileMovementCompPtr->InitialSpeed = 10000.f;
    ProjectileMovementCompPtr->MaxSpeed = 10000.f;
    ProjectileMovementCompPtr->bRotationFollowsVelocity = true;
    ProjectileMovementCompPtr->bShouldBounce = true;

    // Die after 3 seconds by default
    InitialLifeSpan = 1.5f;
}

#ifdef WITH_EDITOR
void AProjectileBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DrawDebugSphere(GetWorld(), GetActorLocation(), 8.f, 10, FColor::Red, false, 10);
}
#endif
