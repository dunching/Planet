
#include "EquipmentGun.h"

#include <thread>

#include "NiagaraComponent.h"
#include "NiagaraActor.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Engine/SkeletalMesh.h"
#include "DrawDebugHelpers.h"
#include "Async/Async.h"

#include "SceneObj/CollisionDataStruct.h"
#include "CharacterBase.h"
#include "Pawn/PawnDataStruct.h"
#include <HumanCharacter.h>
#include <Camera/CameraComponent.h>
#include "AssetRefrencePath.h"
#include "SceneObj/Projectile/ProjectileBase.h"

AEquipmentGun::AEquipmentGun(const FObjectInitializer& ObjectInitializer) :
	Super(
		ObjectInitializer.SetDefaultSubobjectClass<UEquipmentGunInteractionComponent>(UEquipmentGunInteractionComponent::ComponentName)
	)
{
    SceneCompPtr = CreateDefaultSubobject<USceneComponent>(TEXT("CapsuleCompPtr"));

    SceneCompPtr->SetShouldUpdatePhysicsVolume(true);
    SceneCompPtr->SetCanEverAffectNavigation(false);
    RootComponent = SceneCompPtr;

    SkeletalMeshCompPtr = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SimulationLeftSphereComp"));
    SkeletalMeshCompPtr->SetCollisionProfileName(EquipmentItem);
    SkeletalMeshCompPtr->SetSimulatePhysics(false);
    SkeletalMeshCompPtr->SetGenerateOverlapEvents(true);
	SkeletalMeshCompPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMeshCompPtr->SetupAttachment(SceneCompPtr);
}

void AEquipmentGun::EquipItemToCharacter(AHumanCharacter* CharacterPtr)
{
    Super::EquipItemToCharacter(CharacterPtr);
}

void AEquipmentGun::StartAttack()
{

	float Radio = 1.f / GetPropertyComponent<UEquipmentGunInteractionComponent>()->GetAttackSpeed();

	if (Asset)
	{
		GetWorld()->GetTimerManager().SetTimer(FireTImer, this, &AEquipmentGun::Fire, Radio, true, 0);
	}
}

void AEquipmentGun::EndAttack()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTImer);

	if (BulletNiagaraActorPtr)
	{
		auto NiagaraCompPtr = BulletNiagaraActorPtr->GetNiagaraComponent();
		NiagaraCompPtr->SetActive(false);

		Async(EAsyncExecution::Thread, std::bind([&](auto TempPtr) {
			std::this_thread::sleep_for(std::chrono::seconds(DelayDestroyTime));
			AsyncTask(ENamedThreads::GameThread, [=]() {
				TempPtr->Destroy();
				});
			}, BulletNiagaraActorPtr));
	}

	IsFiring = false;
}

void AEquipmentGun::BeginPlay()
{
	Super::BeginPlay();
}

int32 UEquipmentGunInteractionComponent::GetAttackSpeed() const
{
	return AttackSpeed;
}

int32 UEquipmentGunInteractionComponent::GetCurHP()const
{
	return HP;
}

bool UEquipmentGunInteractionComponent::Attack(int32 Val)
{
	HP -= Val;

	if (HP <= 0)
	{
		return true;
	}
	return false;
}

void UEquipmentGunInteractionComponent::Attacked_Implementation()
{
	auto OwnerPtr = GetOwner();
	if (OwnerPtr)
	{
		OwnerPtr->Destroy();
	}
}

void AEquipmentGun::OnHandOverlayBegin(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit
)
{
	if (OtherActor)
	{
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 50, 24, FColor::White, false, 10);
	}
}

void AEquipmentGun::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AEquipmentGun::ProcessPistol(ECMDType CMDType, EItemSocketType ItemSocketType)
{

}

void AEquipmentGun::Fire()
{
    if (SkeletalMeshCompPtr)
	{
		const auto MuzzleSock = GetPropertyComponent<UEquipmentGunInteractionComponent>()->GunMuzzSockName;
		const auto LoopDurationName = GetPropertyComponent<UEquipmentGunInteractionComponent>()->LoopDurationName;
		const auto TargetPosName = GetPropertyComponent<UEquipmentGunInteractionComponent>()->TargetPosName;
		FVector Pos = SkeletalMeshCompPtr->GetSocketLocation(
			MuzzleSock
		);

		if (!IsFiring)
		{
			float Radio = 1.f / GetPropertyComponent<UEquipmentGunInteractionComponent>()->GetAttackSpeed();

			BulletNiagaraActorPtr = GetWorld()->SpawnActor<ANiagaraActor>();

			auto NiagaraCompPtr = BulletNiagaraActorPtr->GetNiagaraComponent();
			NiagaraCompPtr->SetAsset(Asset);
			NiagaraCompPtr->SetFloatParameter(LoopDurationName, Radio);

			BulletNiagaraActorPtr->AttachToComponent(SkeletalMeshCompPtr, FAttachmentTransformRules::KeepRelativeTransform, MuzzleSock);

			IsFiring = true;
		}

		auto NiagaraCompPtr = BulletNiagaraActorPtr->GetNiagaraComponent();

		FMinimalViewInfo DesiredView;
		GetOwnerActor<AHumanCharacter>()->GetCameraComp()->GetCameraView(0, DesiredView);

		auto TargetPos = DesiredView.Location + (DesiredView.Rotation.Vector() * 500);

		NiagaraCompPtr->SetVectorParameter(TargetPosName, TargetPos);

        //Set Spawn Collision Handling Override
        FActorSpawnParameters ActorSpawnParams;
        ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        ActorSpawnParams.Instigator = GetOwnerActor();

        // spawn the projectile at the muzzle
		auto ProjecttilePtr = GetWorld()->SpawnActor<AProjectileBase>(ProjectileClass, Pos, DesiredView.Rotation, ActorSpawnParams);
		ProjecttilePtr->StaticMeshCompPtr->OnComponentHit.AddDynamic(this, &AEquipmentGun::OnHandOverlayBegin);

		DrawDebugLine(GetWorld(), Pos, TargetPos, FColor::Red, false, 10);

		DrawDebugSphere(GetWorld(), TargetPos, 8.f, 10, FColor::Red, false, 10);

    }
}

UEquipmentGunInteractionComponent::UEquipmentGunInteractionComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}
