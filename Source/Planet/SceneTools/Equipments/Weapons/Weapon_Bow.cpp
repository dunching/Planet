
#include "Weapon_Bow.h"

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

AWeapon_Bow::AWeapon_Bow(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SkeletalComponentPtr = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	SkeletalComponentPtr->SetupAttachment(SceneCompPtr);
}

void AWeapon_Bow::AttachToCharacter(ACharacterBase* CharacterPtr)
{
	Super::AttachToCharacter(CharacterPtr);

	// 注意：这里使用Lyra动画，添加到这个插槽之后Transform不正确，还不知道怎么改
	AttachToComponent(CharacterPtr->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Socket);
}

USkeletalMeshComponent* AWeapon_Bow::GetMesh()
{
	return SkeletalComponentPtr;
}
