
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

#include "SPlineActor.h"

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
