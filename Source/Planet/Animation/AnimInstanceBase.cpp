#include "AnimInstanceBase.h"

#include "DrawDebugHelpers.h"

#include "Character/GravityMovementComponent.h"

#include "CharacterBase.h"


void UAnimInstanceBase::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UAnimInstanceBase::ExcuteAnimCMD(EAnimCMDType NewAnimCMDType)
{
	AnimCMDType = NewAnimCMDType;
}

void UAnimInstanceBase::OnAnimationNotify(EAnimationNotify AnimationNotify)
{
	for (auto Iter : AnimationNotifyAry)
	{
		Iter(AnimationNotify);
	}
}

void UAnimInstanceBase::ClearCB()
{
	AnimationNotifyAry.Empty();
}

void UAnimInstanceBase::AddAnimationNotify(const FAnimationNotify& NewAttackIsCompleteCB)
{
	AnimationNotifyAry.Add(NewAttackIsCompleteCB);
}

void UAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto PawnPtr = Cast<ACharacterBase>(TryGetPawnOwner());
	if (PawnPtr)
	{
		const auto PawnVelocity = PawnPtr->GetVelocity();

		const auto ForwardDir = PawnPtr->GetActorForwardVector();
		Forward = FVector::DotProduct(PawnVelocity, ForwardDir);

		const auto RightDir = PawnPtr->GetActorRightVector();
	//	Right = FVector::DotProduct(PawnVelocity, RightDir) / PawnPtr->GetGravityMovementComponent()->GetMaxSpeed();
	}
}

