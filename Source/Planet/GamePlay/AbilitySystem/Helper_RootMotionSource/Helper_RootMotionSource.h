// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "HAL/IConsoleManager.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Class.h"
#include "Engine/NetSerialization.h"
#include "Animation/AnimationAsset.h"
#include "GameFramework/RootMotionSource.h"

#include "Helper_RootMotionSource.generated.h"

enum ERootMotionSource_Priority : uint16
{
	kDefault = 1,
	kMove,
	kFlyAway,
	kTraction,
	kForceMove,
};

class ASPlineActor;
class ATornado;
class ACharacterBase;
USTRUCT()
struct FRootMotionSource_MyConstantForce : public FRootMotionSource_ConstantForce
{
	GENERATED_USTRUCT_BODY()

	virtual void PrepareRootMotion(
		float SimulationTime, 
		float MovementTickTime,
		const ACharacter& Character, 
		const UCharacterMovementComponent& MoveComponent
		) override;
};

USTRUCT()
struct FRootMotionSource_FlyAway : public FRootMotionSource
{
	GENERATED_USTRUCT_BODY()

	FRootMotionSource_FlyAway();

	virtual ~FRootMotionSource_FlyAway();

	virtual FRootMotionSource* Clone() const override;

	virtual bool Matches(const FRootMotionSource* Other) const override;

	virtual void PrepareRootMotion(
		float SimulationTime,
		float MovementTickTime,
		const ACharacter& Character,
		const UCharacterMovementComponent& MoveComponent
	) override;

	float Height = 300.f;

	float RiseDuration = .75f;

	FVector OriginalPt = FVector::ZeroVector;

};

USTRUCT()
struct FRootMotionSource_BySpline : public FRootMotionSource
{
	GENERATED_USTRUCT_BODY()

	FRootMotionSource_BySpline();

	virtual ~FRootMotionSource_BySpline();

	virtual FRootMotionSource* Clone() const override;

	virtual bool Matches(const FRootMotionSource* Other) const override;

	virtual void PrepareRootMotion(
		float SimulationTime,
		float MovementTickTime,
		const ACharacter& Character,
		const UCharacterMovementComponent& MoveComponent
	) override;

	ASPlineActor* SPlineActorPtr = nullptr;

	ACharacterBase* TargetCharacterPtr = nullptr;
};

USTRUCT()
struct FRootMotionSource_ByTornado : public FRootMotionSource
{
	GENERATED_USTRUCT_BODY()

	FRootMotionSource_ByTornado();

	virtual ~FRootMotionSource_ByTornado();

	virtual FRootMotionSource* Clone() const override;

	virtual bool Matches(const FRootMotionSource* Other) const override;

	virtual void PrepareRootMotion(
		float SimulationTime,
		float MovementTickTime,
		const ACharacter& Character,
		const UCharacterMovementComponent& MoveComponent
	) override;
	
	// 如果这个值过小，会导致移动时距离过近从而忽略本次移动 ?
	float OuterRadius = 150.f;
	
	float MaxHeight = 200.f;

	float RotationSpeed = 360.f;

	ATornado* TornadoPtr = nullptr;

	ACharacterBase* TargetCharacterPtr = nullptr;
};
