
#include "Weapon_HandProtection.h"

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
#include "AssetRefrencePath.h"
#include "HumanCharacter.h"
#include "Animation/AnimInstanceBase.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"

AWeapon_HandProtection::AWeapon_HandProtection(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	LeftSkeletalComponentPtr = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftSkeletalComponentPtr"));
	LeftSkeletalComponentPtr->SetupAttachment(SceneCompPtr);

	RightSkeletalComponentPtr = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightSkeletalComponentPtr"));
	RightSkeletalComponentPtr->SetupAttachment(SceneCompPtr);
}

void AWeapon_HandProtection::AttachToCharacter(AHumanCharacter* CharacterPtr)
{
	Super::AttachToCharacter(CharacterPtr);

	AttachToComponent(CharacterPtr->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);

	LeftSkeletalComponentPtr->AttachToComponent(CharacterPtr->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, LeftSocket);
	RightSkeletalComponentPtr->AttachToComponent(CharacterPtr->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, RightSocket);
}