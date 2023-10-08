
#include "EquipmentKnife.h"

#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Engine/SkeletalMesh.h"

#include "SceneObj/CollisionDataStruct.h"
#include "CharacterBase.h"
#include "ArticleBase.h"
#include "HumanCharacter.h"

AEquipmentKnife::AEquipmentKnife(const FObjectInitializer& ObjectInitializer) :
	Super(
		ObjectInitializer.SetDefaultSubobjectClass<UEquipmentKnifeInteractionComponent>(UEquipmentKnifeInteractionComponent::ComponentName)
	)
{
    SceneCompPtr = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

    SceneCompPtr->SetShouldUpdatePhysicsVolume(true);
    SceneCompPtr->SetCanEverAffectNavigation(false);
    RootComponent = SceneCompPtr;

    KnifeSkeletalMeshCompPtr = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SimulationLeftSphereComp"));
    KnifeSkeletalMeshCompPtr->SetCollisionProfileName(EquipmentItem);
    KnifeSkeletalMeshCompPtr->SetSimulatePhysics(false);
    KnifeSkeletalMeshCompPtr->SetGenerateOverlapEvents(false);
	KnifeSkeletalMeshCompPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    KnifeSkeletalMeshCompPtr->OnComponentBeginOverlap.AddDynamic(this, &AEquipmentKnife::OnHandOverlayBegin);
	KnifeSkeletalMeshCompPtr->SetupAttachment(SceneCompPtr);

	Knife_SystemPtr = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Knife_Niagara"));
	Knife_SystemPtr->SetupAttachment(KnifeSkeletalMeshCompPtr, TEXT("TrailSock"));
	Knife_SystemPtr->SetAutoActivate(false);
}

void AEquipmentKnife::EquipItemToCharacter(AHumanCharacter* CharacterPtr)
{
    Super::EquipItemToCharacter(CharacterPtr);

    auto SkeletalMeshCompPtr = CharacterPtr->GetMesh();
    if (SkeletalMeshCompPtr)
    {
        if (KnifeSkeletalMeshCompPtr)
        {
            KnifeSkeletalMeshCompPtr->AttachToComponent(SkeletalMeshCompPtr,
                FAttachmentTransformRules::KeepRelativeTransform, FName(TEXT("Knife_Socket")));
        }
    }
}

void AEquipmentKnife::OnAttackAnimtionStart()
{
	KnifeSkeletalMeshCompPtr->SetGenerateOverlapEvents(true);
	if (Knife_SystemPtr)
	{
		Knife_SystemPtr->SetActive(true);
	}
}

void AEquipmentKnife::OnAttackAnimtionStop()
{
	KnifeSkeletalMeshCompPtr->SetGenerateOverlapEvents(false);
	if (Knife_SystemPtr)
	{
		Knife_SystemPtr->SetActive(false);
	}
}

void AEquipmentKnife::OnHandOverlayBegin(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
	auto ArticlePtr = Cast<ARawMaterialBase>(OtherActor);
	if (ArticlePtr)
	{
		auto PropertyPtr = ArticlePtr->GetPropertyComponent<URawMaterialInteractionComponent>();
		auto ItemPickType = PropertyPtr->GetPickType();

		switch (ItemPickType)
		{
		case EPickType::kNone:
			break;
		case EPickType::kRawMaterial:
		{
		}
		break;
		case EPickType::kInteraction:
			break;
		case EPickType::kCount:
			break;
		default:
			break;
		}
	}
}

UEquipmentKnifeInteractionComponent::UEquipmentKnifeInteractionComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}
