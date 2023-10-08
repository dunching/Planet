
#include "EquipmentAxe.h"

#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"

#include "SceneObj/CollisionDataStruct.h"
#include "CharacterBase.h"
#include "ArticleBase.h"
#include "AssetRefrencePath.h"
#include "HumanCharacter.h"
#include "Animation/AnimInstanceBase.h"

AEquipmentAxe::AEquipmentAxe(const FObjectInitializer& ObjectInitializer) : 
	Super(
		ObjectInitializer.SetDefaultSubobjectClass<UEquipmentAxeInteractionComponent>(UEquipmentAxeInteractionComponent::ComponentName)
	)
{
	SceneCompPtr = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	SceneCompPtr->SetShouldUpdatePhysicsVolume(true);
	SceneCompPtr->SetCanEverAffectNavigation(false);
	RootComponent = SceneCompPtr;

	MeshComponentPtr = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	MeshComponentPtr->SetupAttachment(SceneCompPtr);

	NiagaraAxePtr = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	NiagaraAxePtr->SetupAttachment(MeshComponentPtr);
	NiagaraAxePtr->SetAutoActivate(false);
}

void AEquipmentAxe::OnAttackAnimtionStart()
{
	MeshComponentPtr->SetGenerateOverlapEvents(true);

	NiagaraAxePtr->SetAutoActivate(true);

	if (NiagaraAxePtr)
	{
		NiagaraAxePtr->SetActive(true);
	}
}

void AEquipmentAxe::OnAttackAnimtionStop()
{
	MeshComponentPtr->SetGenerateOverlapEvents(false);

	NiagaraAxePtr->SetAutoActivate(false);

	if (NiagaraAxePtr)
	{
		NiagaraAxePtr->SetActive(false);
	}
}

void AEquipmentAxe::StartAttack()
{
	if (bIsAttacking)
	{
		return;
	}

	bWhetherToKeepAttacking = true;

	auto HumanCharacterPtr = Cast<AHumanCharacter>(GetOwner());
	HumanCharacterPtr->GetAnimationIns()->StartCurrentAnimation();

	MeshComponentPtr->Play(true);
}

void AEquipmentAxe::EndAttack()
{
	bWhetherToKeepAttacking = false;
}

void AEquipmentAxe::EquipItemToCharacter(AHumanCharacter* CharacterPtr)
{
	Super::EquipItemToCharacter(CharacterPtr);

	CharacterPtr->GetAnimationIns()->AddAnimationNotify([&](EAnimationNotify AnimationNotify) {
		switch (AnimationNotify)
		{
		case EAnimationNotify::kAttackBegin:
		{
			OnAttackAnimtionStart();
		}
		break;
		case EAnimationNotify::kAttackEnd:
		{
			OnAttackAnimtionStop();
		}
		break;
		}
		});
}

void AEquipmentAxe::DoAction(EEquipmentActionType ActionType)
{
	switch (ActionType)
	{
	case EEquipmentActionType::kStartAction1:
	{
		StartAttack();
	}
	break;
	case EEquipmentActionType::kStopAction1:
	{
		EndAttack();
	}
	break;
	}
}
