
#include "Helper_RootMotionSource.h"

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "EngineLogs.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Curves/CurveVector.h"
#include "Curves/CurveFloat.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Components/SplineComponent.h"
#include <Kismet/KismetMathLibrary.h>

#include "GravityMovementComponent.h"

#include "SPlineActor.h"
#include "Skill_Active_tornado.h"
#include "CharacterBase.h"
#include "LogWriter.h"

static TAutoConsoleVariable<int32> SkillDrawDebugTornado(
	TEXT("Skill.DrawDebug.Tornado"),
	0,
	TEXT("")
	TEXT(" default: 0"));

void FRootMotionSource_MyConstantForce::PrepareRootMotion
(
	float SimulationTime,
	float MovementTickTime,
	const ACharacter& Character,
	const UCharacterMovementComponent& MoveComponent
)
{
	RootMotionParams.Clear();

	auto GravityMoveComponentPtr = Cast<UGravityMovementComponent>(&MoveComponent);
	if (GravityMoveComponentPtr->bHasBlockResult)
	{
		SetTime(GetTime() + Duration);

		return;
	}

	Super::PrepareRootMotion(SimulationTime, MovementTickTime, Character, MoveComponent);
}

FRootMotionSource_BySpline::FRootMotionSource_BySpline()
{
	Settings.SetFlag(ERootMotionSourceSettingsFlags::DisablePartialEndTick);
}

FRootMotionSource_BySpline::~FRootMotionSource_BySpline()
{

}

FRootMotionSource* FRootMotionSource_BySpline::Clone() const
{
	FRootMotionSource_BySpline* CopyPtr = new FRootMotionSource_BySpline(*this);
	return CopyPtr;
}

bool FRootMotionSource_BySpline::Matches(const FRootMotionSource* Other) const
{
	if (!FRootMotionSource::Matches(Other))
	{
		return false;
	}

	const FRootMotionSource_BySpline* OtherCast = static_cast<const FRootMotionSource_BySpline*>(Other);

	return SPlineActorPtr == OtherCast->SPlineActorPtr;
}

void FRootMotionSource_BySpline::PrepareRootMotion
(
	float SimulationTime,
	float MovementTickTime,
	const ACharacter& Character,
	const UCharacterMovementComponent& MoveComponent
)
{
	RootMotionParams.Clear();

	auto GravityMoveComponentPtr = Cast<UGravityMovementComponent>(&MoveComponent);
	if (GravityMoveComponentPtr->bHasBlockResult)
	{
		SetTime(GetTime() + Duration);

		return;
	}

	FTransform NewTransform;

	if (SPlineActorPtr)
	{
		const float MoveFraction = (GetTime() + SimulationTime) / Duration;

		const float Multiplier = (MovementTickTime > UE_SMALL_NUMBER) ? (SimulationTime / MovementTickTime) : 1.f;

		const auto Pt = SPlineActorPtr->SplineComponentPtr->GetWorldLocationAtTime(MoveFraction, true);

		const FVector CurrentLocation = Character.GetActorLocation();

		FVector Force = (Pt - CurrentLocation) / MovementTickTime;

		NewTransform.SetTranslation(Force);
	}

	RootMotionParams.Set(NewTransform);

	SetTime(GetTime() + SimulationTime);
}

FRootMotionSource_ByTornado::FRootMotionSource_ByTornado()
{
	Settings.SetFlag(ERootMotionSourceSettingsFlags::DisablePartialEndTick);
}

FRootMotionSource_ByTornado::~FRootMotionSource_ByTornado()
{

}

FRootMotionSource* FRootMotionSource_ByTornado::Clone() const
{
	FRootMotionSource_ByTornado* CopyPtr = new FRootMotionSource_ByTornado(*this);
	return CopyPtr;
}

bool FRootMotionSource_ByTornado::Matches(const FRootMotionSource* Other) const
{
	if (!FRootMotionSource::Matches(Other))
	{
		return false;
	}

	const FRootMotionSource_ByTornado* OtherCast = static_cast<const FRootMotionSource_ByTornado*>(Other);

	return TornadoPtr == OtherCast->TornadoPtr;
}

void FRootMotionSource_ByTornado::PrepareRootMotion
(
	float SimulationTime,
	float MovementTickTime,
	const ACharacter& Character,
	const UCharacterMovementComponent& MoveComponent
)
{
	RootMotionParams.Clear();

	FTransform NewTransform;

	if (TornadoPtr)
	{
		const FVector CurrentLocation = Character.GetActorLocation();
		const FVector TornadoLocation = TornadoPtr->GetActorLocation();

		const auto Rotator =
			UKismetMathLibrary::MakeRotFromZX(-Character.GetGravityDirection(), CurrentLocation - TornadoLocation);

		const auto NewRotator =
			Rotator.Vector().RotateAngleAxis(SimulationTime * RotationSpeed, -Character.GetGravityDirection());

		const auto NewPt = 
			TornadoLocation + 
			(-Character.GetGravityDirection() * MaxHeight) +
			(NewRotator * OuterRadius);

		FVector Distance = (NewPt - CurrentLocation) / MovementTickTime;

#ifdef WITH_EDITOR
		if (SkillDrawDebugTornado.GetValueOnGameThread())
		{
			DrawDebugSphere(MoveComponent.GetWorld(), NewPt, 20, 20, FColor::Red, false, 3);
		}
#endif

		NewTransform.SetTranslation(Distance);

		FQuat Rot = FQuat::FindBetween(Character.GetActorForwardVector(), NewRotator) / MovementTickTime;

		NewTransform.SetRotation(Rot);
	}

	RootMotionParams.Set(NewTransform);

	SetTime(GetTime() + SimulationTime);
}
