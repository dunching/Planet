
#include "EquipmentBase.h"

#include "Components/SkeletalMeshComponent.h"

#include "CharacterBase.h"
#include "HumanCharacter.h"
#include "Animation/AnimInstanceBase.h"

AEquipmentBase::AEquipmentBase(const FObjectInitializer& ObjectInitializer) :
	Super(
		ObjectInitializer.SetDefaultSubobjectClass<UEquipmentInteractionComponent>(UEquipmentInteractionComponent::ComponentName)
	)
{

}

void AEquipmentBase::EquipItemToCharacter(AHumanCharacter* CharacterPtr)
{
    OnwerPawnPtr = CharacterPtr;

	SetOwner(CharacterPtr);

	CharacterPtr->GetAnimationIns()->ExcuteAnimCMD(GetPropertyComponent<UEquipmentInteractionComponent>()->GetAnimCMDType());

	auto SkeletalMeshCompPtr = CharacterPtr->GetMesh();
	if (SkeletalMeshCompPtr)
	{
		AttachToComponent(
			SkeletalMeshCompPtr,
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			GetPropertyComponent<UEquipmentInteractionComponent>()->GetSocketName()
		);
	}
}

void AEquipmentBase::DoAction(EEquipmentActionType ActionType)
{

}

void AEquipmentBase::BeginPlay()
{
	Super::BeginPlay();
}

void AEquipmentBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

UEquipmentInteractionComponent::UEquipmentInteractionComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{

}

FName UEquipmentInteractionComponent::GetSocketName() const
{
	return SocksName;
}

void UEquipmentInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	AttackPower = GetInitialAttackPower();
}

int32 UEquipmentInteractionComponent::GetInitialAttackPower_Implementation()
{
	return 10;
}
