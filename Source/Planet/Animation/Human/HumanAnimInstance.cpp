// Copyright Epic Games, Inc. All Rights Reserved.

#include "HumanAnimInstance.h"

#include "CharacterBase.h"
#include "GravityMovementComponent.h"
#include "HorseCharacter.h"
#include "CharacterAttributesComponent.h"
#include "CharacterAttibutes.h"

void UHumanAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	auto PawnPtr = Cast<ACharacterBase>(TryGetPawnOwner());
#if UE_EDITOR || UE_SERVER
	if (PawnPtr)
	{
		if (PawnPtr->GetNetMode() == NM_DedicatedServer)
		{
		}
	}
#endif

	// 绑定一些会影响到 Character行动的数据
	MoveSpeedChangedHandle = PawnPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().MoveSpeed.AddOnValueChanged(
		std::bind(&ThisClass::OnMoveSpeedChanged, this, std::placeholders::_2)
	);
	OnMoveSpeedChanged(PawnPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().MoveSpeed.GetCurrentValue());
}

void UHumanAnimInstance::BeginDestroy()
{
	if (MoveSpeedChangedHandle)
	{
		MoveSpeedChangedHandle->UnBindCallback();
	}

	Super::BeginDestroy();
}

FQuat UHumanAnimInstance::GetGravityToWorldTransform() const
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwningActor());
	if (!Character)
	{
		return FQuat::Identity;
	}
	return Character->GetGravityToWorldTransform();
}

bool UHumanAnimInstance::HasMatchingGameplayTag() const
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwningActor());
	if (!Character)
	{
		return false;
	}
	return Character->GetAbilitySystemComponent()->K2_HasMatchingGameplayTag(TagToCheck);
}

bool UHumanAnimInstance::HasAnyMatchingGameplayTags() const
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwningActor());
	if (!Character)
	{
		return false;
	}
	return Character->GetAbilitySystemComponent()->K2_HasAnyMatchingGameplayTags(TagContainer);
}

UPlanetAbilitySystemComponent* UHumanAnimInstance::GetAbilitySystemComponent() const
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwningActor());
	if (!Character)
	{
		return nullptr;
	}
	return Character->GetAbilitySystemComponent();
}

void UHumanAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ACharacterBase* Character = Cast<ACharacterBase>(GetOwningActor());
	if (!Character)
	{
		return;
	}

	UGravityMovementComponent* CharMoveComp = Character->GetGravityMovementComponent();
	const FLyraCharacterGroundInfo& GroundInfo = CharMoveComp->GetGroundInfo();
	GroundDistance = GroundInfo.GroundDistance;

	auto HorsePtr = Cast<AHorseCharacter>(Character->GetAttachParentActor());
	if (HorsePtr)
	{
		HorseSpeed = HorsePtr->GetVelocity().Length();
	}
}

