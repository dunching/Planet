
#include "ProjectileBase.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include <DrawDebugHelpers.h>
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

#include "CollisionDataStruct.h"
#include "CharacterBase.h"

static TAutoConsoleVariable<int32> ProjectileBase(
    TEXT("Skill.DrawDebug.ProjectileBase"),
    0,
    TEXT("")
    TEXT(" default: 0"));

AProjectileBase::AProjectileBase(const FObjectInitializer& ObjectInitializer) :
    Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 1.f / 60;

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
    ProjectileMovementCompPtr->ProjectileGravityScale = 0.f;

    // Die after 3 seconds by default
    InitialLifeSpan = 1.5f;
}

void AProjectileBase::BeginPlay()
{
    Super::BeginPlay();

    StartPt = GetActorLocation();
}

#ifdef WITH_EDITOR
#endif

void AProjectileBase::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if ((MaxMoveRange > 0) && (FVector::Distance(StartPt, GetActorLocation()) >= MaxMoveRange))
    {
        Destroy();
    }

#ifdef WITH_EDITOR
    if (ProjectileBase.GetValueOnGameThread())
    {
        DrawDebugSphere(GetWorld(), GetActorLocation(), 8.f, 10, FColor::Red, false, 10);
    }
#endif
}

void AProjectileBase::SetHomingTarget_Implementation(ACharacterBase* TargetPtr)
{
    ProjectileMovementCompPtr->bIsHomingProjectile = true;
    ProjectileMovementCompPtr->HomingTargetComponent = TargetPtr->GetRootComponent();
}

void AProjectileBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(ThisClass, MaxMoveRange, COND_InitialOnly);
}
