// Copyright Epic Games, Inc. All Rights Reserved.

#include "HumanAnimInstance.h"

#include "CharacterBase.h"
#include "GravityMovementComponent.h"

#include "HorseCharacter.h"

FQuat UHumanAnimInstance::GetGravityToWorldTransform() const
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwningActor());
	if (!Character)
	{
		return FQuat::Identity;
	}
	return Character->GetGravityToWorldTransform();
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

