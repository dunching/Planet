
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
#include "CS_RootMotion_TornadoTraction.h"
#include "CS_RootMotion_Traction.h"

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

UScriptStruct* FRootMotionSource_MyConstantForce::GetScriptStruct() const
{
	return FRootMotionSource_MyConstantForce::StaticStruct();
}

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
	if (GravityMoveComponentPtr->bSkip_RootMotion)
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

	return
		(StartPtIndex == OtherCast->StartPtIndex) &&
		(EndPtIndex == OtherCast->EndPtIndex) &&
		(SPlineActorPtr == OtherCast->SPlineActorPtr) &&
		(TargetCharacterPtr == OtherCast->TargetCharacterPtr) &&
		FMath::IsNearlyEqual(StartDistance, OtherCast->StartDistance) &&
		FMath::IsNearlyEqual(EndDistance, OtherCast->EndDistance);
}

bool FRootMotionSource_BySpline::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (!Super::NetSerialize(Ar, Map, bOutSuccess))
	{
		return false;
	}

	Ar << StartPtIndex;
	Ar << EndPtIndex;
	Ar << StartDistance;
	Ar << EndDistance;
	Ar << SPlineActorPtr;
	Ar << TargetCharacterPtr;

	bOutSuccess = true;
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

	const FRootMotionSource_BySpline* OtherCast = static_cast<const FRootMotionSource_BySpline*>(SourceToTakeStateFrom);

	StartPtIndex = OtherCast->StartPtIndex;
	EndPtIndex = OtherCast->EndPtIndex;
	StartDistance = OtherCast->StartDistance;
	StartPtIndex = OtherCast->StartPtIndex;
	SPlineActorPtr = OtherCast->SPlineActorPtr;
	TargetCharacterPtr = OtherCast->TargetCharacterPtr;

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

FRootMotionSource* FRootMotionSource_Formation::Clone() const
{
	FRootMotionSource_Formation* CopyPtr = new FRootMotionSource_Formation(*this);
	return CopyPtr;
}

bool FRootMotionSource_Formation::Matches(const FRootMotionSource* Other) const
{
	if (!FRootMotionSource::Matches(Other))
	{
		return false;
	}

	const FRootMotionSource_Formation* OtherCast = static_cast<const FRootMotionSource_Formation*>(Other);

	return FormationPtr == OtherCast->FormationPtr;
}

bool FRootMotionSource_Formation::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (!Super::NetSerialize(Ar, Map, bOutSuccess))
	{
		return false;
	}

	Ar << FormationPtr;

	bOutSuccess = true;
	return true;
}

UScriptStruct* FRootMotionSource_Formation::GetScriptStruct() const
{
	return FRootMotionSource_Formation::StaticStruct();
}

bool FRootMotionSource_Formation::UpdateStateFrom(
	const FRootMotionSource* SourceToTakeStateFrom,
	bool bMarkForSimulatedCatchup /*= false*/
)
{
	if (!FRootMotionSource::UpdateStateFrom(SourceToTakeStateFrom, bMarkForSimulatedCatchup))
	{
		return false;
	}

	const FRootMotionSource_Formation* OtherCast = static_cast<const FRootMotionSource_Formation*>(SourceToTakeStateFrom);

	FormationPtr = OtherCast->FormationPtr;

	return true;
}

void FRootMotionSource_Formation::PrepareRootMotion
(
	float SimulationTime,
	float MovementTickTime,
	const ACharacter& Character,
	const UCharacterMovementComponent& MoveComponent
)
{
	RootMotionParams.Clear();

	if (FormationPtr)
	{
		const FVector CurrentLocation = Character.GetActorLocation();
		const FVector TargetLocation = FormationPtr->GetComponentLocation();

		FVector Distance = (TargetLocation - CurrentLocation) / MovementTickTime;

		FTransform NewTransform;

		NewTransform.SetTranslation(Distance);

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
//		bIsFalling == OtherCast->bIsFalling &&
		RisingSpeed == OtherCast->RisingSpeed &&
		FallingSpeed == OtherCast->FallingSpeed &&
		Height == OtherCast->Height;
}

bool FRootMotionSource_FlyAway::MatchesAndHasSameState(const FRootMotionSource* Other) const
{
	if (!FRootMotionSource::MatchesAndHasSameState(Other))
	{
		return false;
	}

	return true;
}

bool FRootMotionSource_FlyAway::UpdateStateFrom(const FRootMotionSource* SourceToTakeStateFrom, bool bMarkForSimulatedCatchup /*= false*/)
{
	if (!FRootMotionSource::UpdateStateFrom(SourceToTakeStateFrom, bMarkForSimulatedCatchup))
	{
		return false;
	}

	auto OtherCast = static_cast<const FRootMotionSource_FlyAway*>(SourceToTakeStateFrom);

	bIsFalling = OtherCast->bIsFalling;
	RisingSpeed = OtherCast->RisingSpeed;
	FallingSpeed = OtherCast->FallingSpeed;
	Height = OtherCast->Height;

	return true;
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
	const FVector GravityDir = UKismetGravityLibrary::GetGravity(FVector::ZeroVector);

	FCollisionQueryParams Params;
	Params.bTraceComplex = false;
	Params.AddIgnoredActor(&Character);

	const ECollisionChannel CollisionChannel = MoveComponent.UpdatedComponent->GetCollisionObjectType();
	FHitResult Result;
	if (Character.GetWorld()->LineTraceSingleByChannel(
		Result,
		CurrentLocation,
		CurrentLocation + (GravityDir * Line), // 避免找不到
		CollisionChannel,
		Params
	))
	{
		const auto CurrentHeight = FVector::Distance(Result.ImpactPoint, CurrentLocation);

		FVector Force = FVector::ZeroVector;

		// 下降
		if (GetTime() >= GetDuration())
		{
			if (MoveComponent.CurrentFloor.bWalkableFloor && (MoveComponent.CurrentFloor.FloorDist < MoveComponent.MIN_FLOOR_DIST))
			{
				if (Character.GetLocalRole() > ROLE_SimulatedProxy)
				{
					bIsFalling = false;
				}
			}
			else if (FallingSpeed > 0)
			{
				Force.Z = -FallingSpeed;
			}
			else
			{
				const FVector Gravity = -MoveComponent.GetGravityDirection() * MoveComponent.GetGravityZ();
				Force = MoveComponent.NewFallVelocity(Force, Gravity, SimulationTime) / MovementTickTime;
			}
		}
		// 上升
		else if (CurrentHeight < (Height + HalfHeight))
		{
			Force.Z = RisingSpeed;
		}
		// 维持高度
		else
		{
		}

		NewTransform.SetTranslation(Force);
		const float Multiplier = (MovementTickTime > UE_SMALL_NUMBER) ? (SimulationTime / MovementTickTime) : 1.f;
		NewTransform.ScaleTranslation(Multiplier);

		RootMotionParams.Set(NewTransform);
	}

	const auto NewTime = GetTime() + SimulationTime;

	if (Character.GetLocalRole() > ROLE_SimulatedProxy)
	{
		if (NewTime >= GetDuration())
		{
			if (MoveComponent.CurrentFloor.bWalkableFloor && (MoveComponent.CurrentFloor.FloorDist < MoveComponent.MIN_FLOOR_DIST))
			{
				PRINTINVOKEINFO();
			}
			else
			{
				bIsFalling = true;
			}
		}
	}

	SetTime(NewTime);
}

bool FRootMotionSource_FlyAway::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (!FRootMotionSource::NetSerialize(Ar, Map, bOutSuccess))
	{
		return false;
	}

	Ar << RisingSpeed;
	Ar << FallingSpeed;
	Ar << Height;
	Ar << bIsFalling;

	bOutSuccess = true;
	return true;
}

UScriptStruct* FRootMotionSource_FlyAway::GetScriptStruct() const
{
	return FRootMotionSource_FlyAway::StaticStruct();
}

void FRootMotionSource_FlyAway::CheckTimeOut()
{
	if (bIsFalling)
	{
		PRINTINVOKEINFO();
	}
	else
	{
		Super::CheckTimeOut();
	}
}

void FRootMotionSource_FlyAway::Initial(
	float InHeight,
	float InDuration,
	int32 InRisingSpeed,
	int32 InFallingSpeed,
	const FVector& OriginalPt,
	ACharacter* CharacterPtr
)
{
	CharacterPtr->GetCapsuleComponent()->GetScaledCapsuleSize(Radius, HalfHeight);
	FVector CapsuleExtent(Radius, Radius, HalfHeight);

	UpdateDuration(
		InHeight,
		InDuration,
		InRisingSpeed,
		InFallingSpeed,
		OriginalPt
	);
}

void FRootMotionSource_FlyAway::UpdateDuration(
	float InHeight,
	float InDuration,
	int32 InRisingSpeed,
	int32 InFallingSpeed,
	const FVector& InOriginalPt
)
{
	SetTime(0.f);

	Duration = InDuration;
	Height = InHeight;
	RisingSpeed = InRisingSpeed;
	FallingSpeed = InFallingSpeed;
}

void SetRootMotionFinished(FRootMotionSource& RootMotionSource)
{
	RootMotionSource.Status.SetFlag(ERootMotionSourceStatusFlags::Finished);
}

void FRootMotionSource_MyRadialForce::PrepareRootMotion
(
	float SimulationTime,
	float MovementTickTime,
	const ACharacter& Character,
	const UCharacterMovementComponent& MoveComponent
)
{
	if (TractionPointPtr.IsValid())
	{
		Location = TractionPointPtr->GetActorLocation();
	}

	RootMotionParams.Clear();

	const FVector CharacterLocation = Character.GetActorLocation();
	FVector Force = FVector::ZeroVector;
	const FVector ForceLocation = LocationActor ? LocationActor->GetActorLocation() : Location;
	float Distance = FVector::Dist(ForceLocation, CharacterLocation);
	if (
		(Distance < Radius) && 
		(Distance > AcceptableRadius)
		)
	{
		// Calculate strength
		float CurrentStrength = Strength;
		{
			float AdditiveStrengthFactor = 1.f;
			if (StrengthDistanceFalloff)
			{
				const float DistanceFactor = StrengthDistanceFalloff->GetFloatValue(FMath::Clamp(Distance / Radius, 0.f, 1.f));
				AdditiveStrengthFactor -= (1.f - DistanceFactor);
			}

			if (StrengthOverTime)
			{
				const float TimeValue = Duration > 0.f ? FMath::Clamp(GetTime() / Duration, 0.f, 1.f) : GetTime();
				const float TimeFactor = StrengthOverTime->GetFloatValue(TimeValue);
				AdditiveStrengthFactor -= (1.f - TimeFactor);
			}

			CurrentStrength = Strength * FMath::Clamp(AdditiveStrengthFactor, 0.f, 1.f);
		}

		if (bUseFixedWorldDirection)
		{
			Force = FixedWorldDirection.Vector() * CurrentStrength;
		}
		else
		{
			Force = (ForceLocation - CharacterLocation).GetSafeNormal() * CurrentStrength;

			if (bIsPush)
			{
				Force *= -1.f;
			}
		}
	}

	if (bNoZForce)
	{
		Force.Z = 0.f;
	}

	FTransform NewTransform(Force);

	// Scale force based on Simulation/MovementTime differences
	// Ex: Force is to go 200 cm per second forward.
	//     To catch up with server state we need to apply
	//     3 seconds of this root motion in 1 second of
	//     movement tick time -> we apply 600 cm for this frame
	if (SimulationTime != MovementTickTime && MovementTickTime > UE_SMALL_NUMBER)
	{
		const float Multiplier = SimulationTime / MovementTickTime;
		NewTransform.ScaleTranslation(Multiplier);
	}

	RootMotionParams.Set(NewTransform);

	SetTime(GetTime() + SimulationTime);
}

void FRootMotionSource_MyRadialForce::CheckTimeOut()
{
	if (TractionPointPtr.IsValid())
	{
		Super::CheckTimeOut();
	}
	else
	{
		Status.SetFlag(ERootMotionSourceStatusFlags::Finished);
	}
}

bool FRootMotionSource_MyRadialForce::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (!Super::NetSerialize(Ar, Map, bOutSuccess))
	{
		return false;
	}

	Ar << TractionPointPtr;

	bOutSuccess = true;
	return true;
}

UScriptStruct* FRootMotionSource_MyRadialForce::GetScriptStruct() const
{
	return FRootMotionSource_MyRadialForce::StaticStruct();
}

FRootMotionSource* FRootMotionSource_MyRadialForce::Clone() const
{
	FRootMotionSource_MyRadialForce* CopyPtr = new FRootMotionSource_MyRadialForce(*this);
	return CopyPtr;
}

bool FRootMotionSource_MyRadialForce::Matches(const FRootMotionSource* Other) const
{
	if (!Super::Matches(Other))
	{
		return false;
	}

	const auto OtherCast = static_cast<const FRootMotionSource_MyRadialForce*>(Other);

	return
		TractionPointPtr == OtherCast->TractionPointPtr;
}

bool FRootMotionSource_MyRadialForce::MatchesAndHasSameState(const FRootMotionSource* Other) const
{
	if (!Super::MatchesAndHasSameState(Other))
	{
		return false;
	}

	return true;
}

bool FRootMotionSource_MyRadialForce::UpdateStateFrom(const FRootMotionSource* SourceToTakeStateFrom, bool bMarkForSimulatedCatchup /*= false*/)
{
	if (!Super::UpdateStateFrom(SourceToTakeStateFrom, bMarkForSimulatedCatchup))
	{
		return false;
	}

	auto OtherCast = static_cast<const FRootMotionSource_MyRadialForce*>(SourceToTakeStateFrom);

	TractionPointPtr = OtherCast->TractionPointPtr;

	return true;
}
