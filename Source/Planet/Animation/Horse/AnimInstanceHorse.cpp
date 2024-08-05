
#include "AnimInstanceHorse.h"

#include <GameFramework/SpringArmComponent.h>
#include <HumanCharacter.h>
#include "Character/GravityMovementComponent.h"

void UAnimInstanceHorse::SetIsCrounch(bool IsCrounch)
{
    bIsCrounch = IsCrounch;
}

void UAnimInstanceHorse::SetPitch(float Val)
{
    Pitch = Val;
}

void UAnimInstanceHorse::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto PawnPtr = Cast<AHumanCharacter>(TryGetPawnOwner());
	if (PawnPtr)
	{
// 		bIsInAir = PawnPtr->GetGravityMovementComponent()->IsFalling();
// 		
// 		auto SpringArmCompPtr = PawnPtr->GetSpringArmComponent();
// 		if (!SpringArmCompPtr)
// 		{
// 			return;
// 		}
// 		Pitch = SpringArmCompPtr->GetRelativeRotation().Pitch;
	}
}

