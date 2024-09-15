
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
#include <Components/CapsuleComponent.h>

#include "GravityMovementComponent.h"
#include "KismetGravityLibrary.h"

#include "SPlineActor.h"
#include "Skill_Active_tornado.h"
#include "CharacterBase.h"
#include "LogWriter.h"

static TAutoConsoleVariable<int32> SkillDrawDebugTornado(
	TEXT("Skill.DrawDebug.Tornado"),
	0,
	TEXT("")
	TEXT(" default: 0"));

static TAutoConsoleVariable<int32> SkillDrawDebugSpline(
	TEXT("Skill.DrawDebug.Spline"),
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
	if (GravityMoveComponentPtr->bSkipRootMotion)
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

	return true;
// 		(StartPtIndex == OtherCast->StartPtIndex) &&
// 		(EndPtIndex == OtherCast->EndPtIndex) &&
// 		(SPlineActorPtr == OtherCast->SPlineActorPtr) &&
// 		(TargetCharacterPtr == OtherCast->TargetCharacterPtr) &&
// 		FMath::IsNearlyEqual(StartDistance, OtherCast->StartDistance) &&
// 		FMath::IsNearlyEqual(EndDistance, OtherCast->EndDistance) ;
}

bool FRootMotionSource_BySpline::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (!Super::NetSerialize(Ar, Map, bOutSuccess))
	{
		return false;
	}

// 	Ar << StartPtIndex; 
// 	Ar << EndPtIndex;
// 	Ar << StartDistance;
// 	Ar << EndDistance;
// 	Ar << SPlineActorPtr;
// 	Ar << TargetCharacterPtr;
// 
// 	bOutSuccess = true;
	return true;
}

UScriptStruct* FRootMotionSource_BySpline::GetScriptStruct() const
{
	return FRootMotionSource_BySpline::StaticStruct();
}

bool FRootMotionSource_BySpline::UpdateStateFrom(
	const FRootMotionSource* SourceToTakeStateFrom,
	bool bMarkForSimulatedCatchup /*= false*/
)
{
	if (!FRootMotionSource::UpdateStateFrom(SourceToTakeStateFrom, bMarkForSimulatedCatchup))
	{
		return false;
	}

// 	const FRootMotionSource_BySpline* OtherCast = static_cast<const FRootMotionSource_BySpline*>(SourceToTakeStateFrom);
// 
// 	StartPtIndex = OtherCast->StartPtIndex;
// 	EndPtIndex = OtherCast->EndPtIndex;
// 	StartDistance = OtherCast->StartDistance;
// 	StartPtIndex = OtherCast->StartPtIndex;
// 	SPlineActorPtr = OtherCast->SPlineActorPtr;
// 	TargetCharacterPtr = OtherCast->TargetCharacterPtr;

	return true;
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
	if (GravityMoveComponentPtr->PerformBlockResult.bBlockingHit)
	{
		SetTime(GetTime() + Duration);

		return;
	}

	FTransform NewTransform;

	if (SPlineActorPtr.IsValid())
	{
		const float MoveFraction = (GetTime() + SimulationTime) / Duration;

		const auto LerpValue = FMath::Lerp(StartDistance, EndDistance, MoveFraction);

		const auto Pt = SPlineActorPtr->SplineComponentPtr->GetWorldLocationAtDistanceAlongSpline(LerpValue);

#ifdef WITH_EDITOR
		if (SkillDrawDebugSpline.GetValueOnGameThread())
		{
			DrawDebugSphere(MoveComponent.GetWorld(), Pt, 20, 20, FColor::Red, false, 3);

			PRINTINVOKEWITHSTR(FString::Printf(TEXT("SimulationTime:%.2lf"), SimulationTime));
			PRINTINVOKEWITHSTR(FString::Printf(TEXT("MovementTickTime:%.2lf"), MovementTickTime));
		}
#endif

		const FVector CurrentLocation = Character.GetActorLocation();

		FVector Force = (Pt - CurrentLocation) / MovementTickTime;

		NewTransform.SetTranslation(Force);
	}

	const float Multiplier = (MovementTickTime > UE_SMALL_NUMBER) ? (SimulationTime / MovementTickTime) : 1.f;
	NewTransform.ScaleTranslation(Multiplier);
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

	if (TornadoPtr.IsValid())
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

		FTransform NewTransform;

		NewTransform.SetTranslation(Distance);

		FQuat Rot = FQuat::FindBetween(Character.GetActorForwardVector(), NewRotator) / MovementTickTime;

		NewTransform.SetRotation(Rot);

		const float Multiplier = (MovementTickTime > UE_SMALL_NUMBER) ? (SimulationTime / MovementTickTime) : 1.f;
		NewTransform.ScaleTranslation(Multiplier);

		RootMotionParams.Set(NewTransform);

		SetTime(GetTime() + SimulationTime);
	}
	else
	{
		SetRootMotionFinished(*this);
	}
}

FRootMotionSource_FlyAway::FRootMotionSource_FlyAway()
{
	Settings.SetFlag(ERootMotionSourceSettingsFlags::DisablePartialEndTick);
}

FRootMotionSource_FlyAway::~FRootMotionSource_FlyAway()
{

}

FRootMotionSource* FRootMotionSource_FlyAway::Clone() const
{
	FRootMotionSource_FlyAway* CopyPtr = new FRootMotionSource_FlyAway(*this);
	return CopyPtr;
}

bool FRootMotionSource_FlyAway::Matches(const FRootMotionSource* Other) const
{
	if (!FRootMotionSource::Matches(Other))
	{
		return false;
	}

	const auto OtherCast = static_cast<const FRootMotionSource_FlyAway*>(Other);

	return 
		FMath::IsNearlyEqual(RiseDuration, OtherCast->RiseDuration) ||
		TargetPt == OtherCast->TargetPt ||
		CurrentStartPt == OtherCast->CurrentStartPt ||
		OriginalLandingPt == OtherCast->OriginalLandingPt;
}

void FRootMotionSource_FlyAway::PrepareRootMotion(
	float SimulationTime,
	float MovementTickTime,
	const ACharacter& Character,
	const UCharacterMovementComponent& MoveComponent
)
{
	RootMotionParams.Clear();

	FTransform NewTransform = FTransform::Identity;

	const FVector CurrentLocation = Character.GetActorLocation();

	if (GetTime() < RiseDuration)
	{
		float MoveFraction = (GetTime() + SimulationTime) / RiseDuration;

		FVector CurrentTargetLocation = FMath::Lerp<FVector, float>(CurrentStartPt, TargetPt, MoveFraction);

		FVector Force = (CurrentTargetLocation - CurrentLocation) / MovementTickTime;

		NewTransform.SetTranslation(Force);
	}
	else
	{
		FVector Force = (TargetPt - CurrentLocation) / MovementTickTime;

		NewTransform.SetTranslation(Force);
	}

	const float Multiplier = (MovementTickTime > UE_SMALL_NUMBER) ? (SimulationTime / MovementTickTime) : 1.f;
	NewTransform.ScaleTranslation(Multiplier);

	RootMotionParams.Set(NewTransform);

	SetTime(GetTime() + SimulationTime);
}

void FRootMotionSource_FlyAway::Initial(
	float Height,
	float InDuration,
	const FVector& OriginalPt
)
{
	OriginalLandingPt = OriginalPt;
	UpdateDuration(Height, InDuration, OriginalPt);
}

void FRootMotionSource_FlyAway::UpdateDuration(
	float InHeight,
	float InDuration,
	const FVector& InOriginalPt
)
{
	Duration = InDuration;

	SetTime(0.f);

	CurrentStartPt = InOriginalPt;

	TargetPt = OriginalLandingPt - (UKismetGravityLibrary::GetGravity(OriginalLandingPt) * InHeight);
}

void SetRootMotionFinished(FRootMotionSource& RootMotionSource)
{
	RootMotionSource.Status.SetFlag(ERootMotionSourceStatusFlags::Finished);
}
