
#include "Weapon_PickAxe.h"

#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"

#include "CollisionDataStruct.h"
#include "ArticleBase.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "Skill_WeaponActive_PickAxe.h"

AWeapon_PickAxe::AWeapon_PickAxe(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SkeletalComponentPtr = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	SkeletalComponentPtr->SetupAttachment(SceneCompPtr);
}

void AWeapon_PickAxe::AttachToCharacter(FOnwerType* CharacterPtr)
{
	Super::AttachToCharacter(CharacterPtr);

	AttachToComponent(CharacterPtr->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Socket);
}

USkeletalMeshComponent* AWeapon_PickAxe::GetMesh()
{
	return SkeletalComponentPtr;
}
