
#include "EquipmentHand.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Animation/AnimInstance.h"
#include "Engine/SkeletalMesh.h"

#include "CollisionDataStruct.h"
#include "ArticleBase.h"
#include "SceneObj/SceneObj.h"
#include "HumanCharacter.h"
#include "Animation/AnimInstanceBase.h"
#include "AssetRefrencePath.h"
#include "InputProcessorSubSystem.h"

AEquipmentHand::AEquipmentHand(const FObjectInitializer& ObjectInitializer) :
	Super(
		ObjectInitializer.SetDefaultSubobjectClass<UEquipmentHandInteractionComponent>(UEquipmentHandInteractionComponent::ComponentName)
	)
{
    SceneCompPtr = CreateDefaultSubobject<USceneComponent>(TEXT("CapsuleCompPtr"));

    SceneCompPtr->SetShouldUpdatePhysicsVolume(true);
    SceneCompPtr->SetCanEverAffectNavigation(false);
    RootComponent = SceneCompPtr;

	SimulationLFistPtr = CreateDefaultSubobject<USphereComponent>(TEXT("SimulationLeftSphereComp"));
	SimulationLFistPtr->SetupAttachment(RootComponent);
	SimulationLFistPtr->OnComponentBeginOverlap.AddDynamic(this, &AEquipmentHand::OnHandOverlayBegin);

	SimulationRFistPtr = CreateDefaultSubobject<USphereComponent>(TEXT("SimulationRightSphereComp"));
	SimulationRFistPtr->SetupAttachment(RootComponent);
	SimulationRFistPtr->OnComponentBeginOverlap.AddDynamic(this, &AEquipmentHand::OnHandOverlayBegin);

	NiagaraLeftHandPtr = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LeftHandTrailNiagara"));
	NiagaraLeftHandPtr->SetupAttachment(SimulationLFistPtr);
	NiagaraLeftHandPtr->SetAutoActivate(false);

	NiagaraRightHandPtr = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RightHandTrailNiagara"));
	NiagaraRightHandPtr->SetupAttachment(SimulationRFistPtr);
	NiagaraRightHandPtr->SetAutoActivate(false);
}

void AEquipmentHand::Destroyed()
{
	UInputProcessorSubSystem::GetInstance()->RemoveKeyEvent(KeyEventDelegateHandle);

	Super::Destroyed();
}

void AEquipmentHand::EquipItemToCharacter(AHumanCharacter* CharacterPtr)
{
	Super::EquipItemToCharacter(CharacterPtr);

	auto SkeletalMeshCompPtr = CharacterPtr->GetMesh();
	if (SkeletalMeshCompPtr)
	{
		if (SimulationLFistPtr)
		{
			SimulationLFistPtr->AttachToComponent(
				SkeletalMeshCompPtr,
				FAttachmentTransformRules::KeepRelativeTransform,
				GetPropertyComponent<UEquipmentHandInteractionComponent>()->LeftHandSockName
			);


		}

		if (SimulationRFistPtr)
		{
			SimulationRFistPtr->AttachToComponent(
				SkeletalMeshCompPtr,
				FAttachmentTransformRules::KeepRelativeTransform,
				GetPropertyComponent<UEquipmentHandInteractionComponent>()->RightHandSockName
			);
		}

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

		KeyEventDelegateHandle = UInputProcessorSubSystem::GetInstance()->AddKeyEvent(EKeys::LeftMouseButton, [this](EInputEvent bIsPressed)
			{
				if (bIsPressed == IE_Pressed)
				{
					StartAttack();
				}
				else
				{
					EndAttack();
				}
			});
	}
}

void AEquipmentHand::OnAttackAnimtionStart()
{
	bIsAttacking = true;

	SimulationLFistPtr->SetGenerateOverlapEvents(true);
	SimulationRFistPtr->SetGenerateOverlapEvents(true);

	NiagaraLeftHandPtr->SetAutoActivate(true);
	NiagaraRightHandPtr->SetAutoActivate(true);

	if (NiagaraLeftHandPtr)
	{
		NiagaraLeftHandPtr->SetActive(true);
	}
	if (NiagaraRightHandPtr)
	{
		NiagaraRightHandPtr->SetActive(true);
	}
}

void AEquipmentHand::OnAttackAnimtionStop()
{
	bIsAttacking = false;

	if (bWhetherToKeepAttacking)
	{
		StartAttack();
	}

	SimulationLFistPtr->SetGenerateOverlapEvents(false);
	SimulationRFistPtr->SetGenerateOverlapEvents(false);

	NiagaraLeftHandPtr->SetAutoActivate(false);
	NiagaraRightHandPtr->SetAutoActivate(false);

	if (NiagaraLeftHandPtr)
	{
		NiagaraLeftHandPtr->SetActive(false);
	}
	if (NiagaraRightHandPtr)
	{
		NiagaraRightHandPtr->SetActive(false);
	}
}

void AEquipmentHand::OnHandOverlayBegin(
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

void AEquipmentHand::StartAttack()
{
	if (bIsAttacking)
	{
		return;
	}

	bWhetherToKeepAttacking = true;
	
	auto HumanCharacterPtr = Cast<AHumanCharacter>(GetOwner());
	HumanCharacterPtr->GetAnimationIns()->StartCurrentAnimation();
}

void AEquipmentHand::EndAttack()
{
	bWhetherToKeepAttacking = false;
}
