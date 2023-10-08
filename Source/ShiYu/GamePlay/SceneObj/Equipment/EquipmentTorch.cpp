
#include "EquipmentTorch.h"

#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Engine/SkeletalMesh.h"

#include "SceneObj/CollisionDataStruct.h"
#include "CharacterBase.h"
#include "ArticleBase.h"
#include "HumanCharacter.h"

AEquipmentTorch::AEquipmentTorch(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
    SceneCompPtr = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

    SceneCompPtr->SetShouldUpdatePhysicsVolume(true);
    SceneCompPtr->SetCanEverAffectNavigation(false);
    RootComponent = SceneCompPtr;

    TorchStaticMeshCompPtr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TorchStaticMeshCompPtr"));
    TorchStaticMeshCompPtr->SetCollisionProfileName(EquipmentItem);
    TorchStaticMeshCompPtr->SetSimulatePhysics(false);
    TorchStaticMeshCompPtr->SetGenerateOverlapEvents(false);
    TorchStaticMeshCompPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    TorchStaticMeshCompPtr->SetupAttachment(SceneCompPtr);

	Knife_SystemPtr = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Knife_SystemPtr->SetupAttachment(TorchStaticMeshCompPtr);
	Knife_SystemPtr->SetAutoActivate(true);
}

void AEquipmentTorch::EquipItemToCharacter(AHumanCharacter* CharacterPtr)
{
    Super::EquipItemToCharacter(CharacterPtr);

    auto SkeletalMeshCompPtr = CharacterPtr->GetMesh();
    if (SkeletalMeshCompPtr)
	{
		AttachToComponent(SkeletalMeshCompPtr,
			FAttachmentTransformRules::KeepRelativeTransform, FName(TEXT("Torch_Socket")));
    }
}
