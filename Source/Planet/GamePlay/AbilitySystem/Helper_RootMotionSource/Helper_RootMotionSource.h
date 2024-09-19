// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Skill_Active_IceGun.h"

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
	kMove,			// 位移
	kFlyAway,		// 击飞
	kTraction,		// 牵引
	kForceMove,		// 强制位移
};

class ASPlineActor;
class ATornado;
class ACharacterBase;

void SetRootMotionFinished(FRootMotionSource& RootMotionSource);

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

	virtual bool MatchesAndHasSameState(const FRootMotionSource* Other) const override;

	virtual bool UpdateStateFrom(const FRootMotionSource* SourceToTakeStateFrom, bool bMarkForSimulatedCatchup = false) override;

	virtual void PrepareRootMotion(
		float SimulationTime,
		float MovementTickTime,
		const ACharacter& Character,
		const UCharacterMovementComponent& MoveComponent
	) override;

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;

	virtual UScriptStruct* GetScriptStruct() const override;

	void Initial(
		float Height,
		float Duration,
		const FVector& OriginalPt,
		ACharacter*CharacterPtr
	);

	void UpdateDuration(
		float Height,
		float Duration,
		const FVector& OriginalPt
	);

	float RiseDuration = .75f;

	int32 Height = 100;

	float Radius = 0.f;

	float HalfHeight = 0.f;
};

template<>
struct TStructOpsTypeTraits< FRootMotionSource_FlyAway > :
	public TStructOpsTypeTraitsBase2< FRootMotionSource_FlyAway >
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};

USTRUCT()
struct FRootMotionSource_BySpline : public FRootMotionSource
{
	GENERATED_USTRUCT_BODY()

	FRootMotionSource_BySpline();

	virtual ~FRootMotionSource_BySpline();

	virtual FRootMotionSource* Clone() const override;

	virtual bool Matches(const FRootMotionSource* Other) const override;

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool UpdateStateFrom(const FRootMotionSource* SourceToTakeStateFrom, bool bMarkForSimulatedCatchup = false) override;

	virtual void PrepareRootMotion(
		float SimulationTime,
		float MovementTickTime,
		const ACharacter& Character,
		const UCharacterMovementComponent& MoveComponent
	) override;

	int32 StartPtIndex = 0;

	int32 EndPtIndex = 1;

	float StartDistance = 0.f;

	float EndDistance = 0.f;

	TWeakObjectPtr<ASPlineActor> SPlineActorPtr = nullptr;

	ACharacterBase* TargetCharacterPtr = nullptr;

private:

};

template<>
struct TStructOpsTypeTraits< FRootMotionSource_BySpline > :
	public TStructOpsTypeTraitsBase2< FRootMotionSource_BySpline >
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
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

	TWeakObjectPtr<ATornado> TornadoPtr = nullptr;

	ACharacterBase* TargetCharacterPtr = nullptr;
};