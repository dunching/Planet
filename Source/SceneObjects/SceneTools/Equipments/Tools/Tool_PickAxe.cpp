
#include "Tool_PickAxe.h"

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

ATool_PickAxe::ATool_PickAxe(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SkeletalComponentPtr = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	SkeletalComponentPtr->SetupAttachment(SceneCompPtr);
}

void ATool_PickAxe::DoActionByCharacter(FOnwerType* CharacterPtr, EEquipmentActionType ActionType)
{
	Super::DoActionByCharacter(CharacterPtr, ActionType);

	switch (ActionType)
	{
	case EEquipmentActionType::kStartAction:
	{
		FGameplayEventData Payload;
		auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_ToolFuture_PickAxe;
		GameplayAbilityTargetDataPtr->EquipmentAxePtr = this;
		Payload.TargetData.Add(GameplayAbilityTargetDataPtr);

		auto ASCPtr = CharacterPtr->GetAbilitySystemComponent();
		ASCPtr->TriggerAbilityFromGameplayEvent(
			EquipmentAbilitieHandle,
			ASCPtr->AbilityActorInfo.Get(),
			FGameplayTag(),
			&Payload,
			*ASCPtr
		);
	}
	break;
	case EEquipmentActionType::kStopAction:
	{
		CharacterPtr->GetAbilitySystemComponent()->CancelAbilityHandle(EquipmentAbilitieHandle);
	}
	break;
	}
}

void ATool_PickAxe::AttachToCharacter(FOnwerType* CharacterPtr)
{
	Super::AttachToCharacter(CharacterPtr);

	AttachToComponent(CharacterPtr->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Socket);
}

USkeletalMeshComponent* ATool_PickAxe::GetMesh()
{
	return SkeletalComponentPtr;
}
