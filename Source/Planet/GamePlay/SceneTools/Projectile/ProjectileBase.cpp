#include "ProjectileBase.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CollisionDataStruct.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include <DrawDebugHelpers.h>

AProjectileBase::AProjectileBase(const FObjectInitializer& ObjectInitializer) :
    Super(ObjectInitializer)
{
	SceneCompPtr = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	RootComponent = SceneCompPtr;

    // Use a sphere as a simple collision representation
    StaticMeshCompPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
    StaticMeshCompPtr->SetCollisionProfileName(ProjecttileItem);
    StaticMeshCompPtr->SetSimulatePhysics(false);
    StaticMeshCompPtr->SetGenerateOverlapEvents(true);
    StaticMeshCompPtr->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    StaticMeshCompPtr->SetMobility(EComponentMobility::Movable);

    // Players can't walk on it
    StaticMeshCompPtr->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	StaticMeshCompPtr->CanCharacterStepUpOn = ECB_No;
    StaticMeshCompPtr->SetupAttachment(SceneCompPtr);

    // Use a ProjectileMovementComponent to govern this projectile's movement
    ProjectileMovementCompPtr = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovementCompPtr->UpdatedComponent = SceneCompPtr;
    ProjectileMovementCompPtr->InitialSpeed = 10000.f;
    ProjectileMovementCompPtr->MaxSpeed = 10000.f;
    ProjectileMovementCompPtr->bRotationFollowsVelocity = true;
    ProjectileMovementCompPtr->bShouldBounce = true;

    // Die after 3 seconds by default
    InitialLifeSpan = 3.0f;
}

#ifdef WITH_EDITOR
void AProjectileBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DrawDebugSphere(GetWorld(), GetActorLocation(), 8.f, 10, FColor::Red, false, 10);
}
#endif
