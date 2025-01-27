#include "Weapon_Bow.h"

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

#include "CollisionDataStruct.h"
#include "CharacterBase.h"
#include "ArticleBase.h"
#include "StateTagExtendInfo.h"
#include "HumanCharacter.h"
#include "Animation/AnimInstanceBase.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "Skill_WeaponActive_PickAxe.h"

ASkill_WeaponActive_Bow_Projectile::ASkill_WeaponActive_Bow_Projectile(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(CollisionComp);

	ProjectileMovementCompPtr->HomingAccelerationMagnitude = 10000.f;
	ProjectileMovementCompPtr->InitialSpeed = 100.f;
	ProjectileMovementCompPtr->MaxSpeed = 100.f;
	InitialLifeSpan = 10.f;
}

AWeapon_Bow::AWeapon_Bow(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SetReplicates(true);
	SetReplicatingMovement(true);
}

void AWeapon_Bow::AttachToCharacter(ACharacterBase* CharacterPtr)
{
	Super::AttachToCharacter(CharacterPtr);

	// 注意：这里使用Lyra动画，添加到这个插槽之后Transform不正确，还不知道怎么改
	AttachToComponent(CharacterPtr->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);

	BowActorPtr->AttachToComponent(
		CharacterPtr->GetMesh(),
		FAttachmentTransformRules::KeepRelativeTransform,
	                               Bow_Socket);
	
	QuiverActorPtr->AttachToComponent(
		CharacterPtr->GetMesh(),
		FAttachmentTransformRules::KeepRelativeTransform,
	                                  Quiver_Socket
	                                  );
	
	ArrowActorPtr->AttachToComponent(
		BowActorPtr->GetSkeletalMeshComponent(),
	                                 FAttachmentTransformRules::KeepRelativeTransform,
	                                 Arrow_Socket
	                                 );
}

USkeletalMeshComponent* AWeapon_Bow::GetMesh() const
{
	return BowActorPtr->GetSkeletalMeshComponent();
}

FTransform AWeapon_Bow::GetEmitTransform() const
{
	return GetMesh()->GetSocketTransform(Arrow_Socket, ERelativeTransformSpace::RTS_World);
}

void AWeapon_Bow::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = GetOwner();

		BowActorPtr = GetWorld()->SpawnActor<ASkeletalMeshActor>(Bow_Class, BowIdleTransform, SpawnParameters);
		BowActorPtr->SetReplicates(true);
		BowActorPtr->SetReplicatingMovement(true);

		QuiverActorPtr = GetWorld()->SpawnActor<AStaticMeshActor>(Quiver_Class, SpawnParameters);
		QuiverActorPtr->SetReplicates(true);
		QuiverActorPtr->SetReplicatingMovement(true);

		ArrowActorPtr = GetWorld()->SpawnActor<AStaticMeshActor>(Arrow_Class, ArrowTransform, SpawnParameters);
		ArrowActorPtr->SetReplicates(true);
		ArrowActorPtr->SetReplicatingMovement(true);
	}
#endif
}

void AWeapon_Bow::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		if (BowActorPtr)
		{
			BowActorPtr->Destroy();
			BowActorPtr = nullptr;
		}

		if (QuiverActorPtr)
		{
			QuiverActorPtr->Destroy();
			QuiverActorPtr = nullptr;
		}

		if (ArrowActorPtr)
		{
			ArrowActorPtr->Destroy();
			ArrowActorPtr = nullptr;
		}
	}
#endif

	Super::EndPlay(EndPlayReason);
}

void AWeapon_Bow::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, BowActorPtr);
	DOREPLIFETIME(ThisClass, QuiverActorPtr);
	DOREPLIFETIME(ThisClass, ArrowActorPtr);
}
