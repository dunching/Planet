
#include "Weapon_PickAxe.h"

#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"

#include "CollisionDataStruct.h"
#include "CharacterBase.h"
#include "ArticleBase.h"
#include "StateTagExtendInfo.h"
#include "HumanCharacter.h"
#include "Animation/AnimInstanceBase.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "Skill_WeaponActive_PickAxe.h"

AWeapon_PickAxe::AWeapon_PickAxe(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SkeletalComponentPtr = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	SkeletalComponentPtr->SetupAttachment(RootComponent);
}

void AWeapon_PickAxe::AttachToCharacterBase(ACharacterBase* CharacterPtr)
{
	Super::AttachToCharacterBase(CharacterPtr);

	AttachToComponent(CharacterPtr->GetCopyPoseMesh(), FAttachmentTransformRules::KeepRelativeTransform, Socket);
}

USkeletalMeshComponent* AWeapon_PickAxe::GetMesh() const
{
	return SkeletalComponentPtr;
}
