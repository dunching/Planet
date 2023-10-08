
#include "AnimInstanceHuman.h"

#include <GameFramework/SpringArmComponent.h>
#include <HumanCharacter.h>
#include "Character/GravityMovementComponent.h"

void UAnimInstanceHuman::SetIsCrounch(bool IsCrounch)
{
    bIsCrounch = IsCrounch;
}

void UAnimInstanceHuman::SetPitch(float Val)
{
    Pitch = Val;
}

void UAnimInstanceHuman::NativeUpdateAnimation(float DeltaSeconds)
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
