
#include "Weapon_FoldingFan.h"

#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"
#include "Animation/SkeletalMeshActor.h"
#include "Engine/StaticMeshActor.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "CollisionDataStruct.h"
#include "CharacterBase.h"
#include "ArticleBase.h"
#include "StateTagExtendInfo.h"
#include "HumanCharacter.h"
#include "Animation/AnimInstanceBase.h"
#include "AbilityTask_PlayMontage.h"
#include "ItemProxy_Weapon.h"
#include "ToolFuture_PickAxe.h"
#include "Skill_WeaponActive_PickAxe.h"
#include "KismetGravityLibrary.h"

AWeapon_FoldingFan::AWeapon_FoldingFan(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<USphereComponent>(ThisClass::RootComponentName))
{
	SetReplicates(true);
	SetReplicatingMovement(true);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f / 60;

	SkeletalComponentPtr = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalComponentPtr"));
	SkeletalComponentPtr->SetupAttachment(RootComponent);

	ProjectileMovementCompPtr = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovementCompPtr->UpdatedComponent = RootComponent;
	ProjectileMovementCompPtr->InitialSpeed = 1000.f;
	ProjectileMovementCompPtr->MaxSpeed = 1000.f;
	ProjectileMovementCompPtr->HomingAccelerationMagnitude = 10000.f;
	ProjectileMovementCompPtr->bRotationFollowsVelocity = true;
	ProjectileMovementCompPtr->bShouldBounce = true;
	ProjectileMovementCompPtr->ProjectileGravityScale = 0.f;
}

void AWeapon_FoldingFan::AttachToCharacter(ACharacter* CharacterPtr)
{
	Super::AttachToCharacter(CharacterPtr);

	EndAttack();

	SetActorRelativeTransform(FTransform::Identity);
	AttachToComponent(CharacterPtr->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Socket);
}

USkeletalMeshComponent* AWeapon_FoldingFan::GetMesh()const
{
	return SkeletalComponentPtr;
}

void AWeapon_FoldingFan::BeginAttack_Implementation()
{
	ProjectileMovementCompPtr->bIsHomingProjectile = false;
	ProjectileMovementCompPtr->SetActive(true);

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

#if UE_EDITOR || UE_SERVER
	if (GetLocalRole() == ROLE_Authority)
	{
		auto Vec = WeaponProxyPtr->GetAllocationCharacter()->GetActorRotation().Vector();
// 		auto Vec = UKismetMathLibrary::MakeRotFromZX(
// 			-UKismetGravityLibrary::GetGravity(), 
// 			WeaponProxyPtr->GetAllocationCharacter()->GetControlRotation().Vector()
// 		).Vector();
		Vec = Vec.RotateAngleAxis(10.f, Vec.ToOrientationQuat().GetRightVector()) * ProjectileMovementCompPtr->InitialSpeed;
// 
		const auto Transform = GetActorTransform();
		ProjectileMovementCompPtr->SetVelocityInLocalSpace(Transform.InverseTransformVector(Vec));
//		ProjectileMovementCompPtr->SetVelocityInLocalSpace(Vec);

		StartPt = GetActorLocation();
		MaxMoveRange = WeaponProxyPtr->GetMaxAttackDistance();
	}
#endif
}

void AWeapon_FoldingFan::EndAttack_Implementation()
{
#if UE_EDITOR || UE_SERVER
	if (GetLocalRole() == ROLE_Authority)
	{
	}
#endif
	GetCollisionComponent()->OnComponentBeginOverlap.Clear();
	ProjectileMovementCompPtr->SetActive(false);
}

USphereComponent* AWeapon_FoldingFan::GetCollisionComponent()
{
	return Cast<USphereComponent>(RootComponent);
}

void AWeapon_FoldingFan::BeginPlay()
{
	Super::BeginPlay();

	EndAttack();
}

void AWeapon_FoldingFan::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AWeapon_FoldingFan::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

#if UE_EDITOR || UE_SERVER
	if (GetLocalRole() == ROLE_Authority)
	{
		if ((MaxMoveRange > 0) && (FVector::Distance(StartPt, GetActorLocation()) >= MaxMoveRange))
		{
			OnReachFarestPoint();
		}
	}
#endif
}

void AWeapon_FoldingFan::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AWeapon_FoldingFan::OnReachFarestPoint_Implementation()
{
#if UE_EDITOR || UE_SERVER
	if (GetLocalRole() > ROLE_SimulatedProxy)
	{
		bIsReachFarestPoint = true;

		ProjectileMovementCompPtr->bIsHomingProjectile = true;
		ProjectileMovementCompPtr->HomingTargetComponent = WeaponProxyPtr->GetAllocationCharacter()->GetRootComponent();
	}
#endif
}
