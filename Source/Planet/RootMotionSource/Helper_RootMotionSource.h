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

	kAIMove,

	kMove, // 位移
	kFlyAway, // 飞行
	kHasBeenFlyAway, // 被击飞
	kTraction, // 牵引
	kForceMove, // 强制位移
};

class ASPlineActor;
class ATornado;
class ACharacterBase;
class ATractionPoint;

void SetRootMotionFinished(
	FRootMotionSource& RootMotionSource
);

USTRUCT()
struct FRootMotionSource_MyConstantForce : public FRootMotionSource_ConstantForce
{
	GENERATED_USTRUCT_BODY()

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(
		FArchive& Ar,
		UPackageMap* Map,
		bool& bOutSuccess
	) override;

	virtual void PrepareRootMotion(
		float SimulationTime,
		float MovementTickTime,
		const ACharacter& Character,
		const UCharacterMovementComponent& MoveComponent
	) override;
};

template <>
struct TStructOpsTypeTraits<FRootMotionSource_MyConstantForce> :
	public TStructOpsTypeTraitsBase2<FRootMotionSource_MyConstantForce>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};

USTRUCT()
struct FRootMotionSource_MyRadialForce : public FRootMotionSource_RadialForce
{
	GENERATED_USTRUCT_BODY()

	virtual FRootMotionSource* Clone() const override;

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool NetSerialize(
		FArchive& Ar,
		UPackageMap* Map,
		bool& bOutSuccess
	) override;

	virtual bool Matches(
		const FRootMotionSource* Other
	) const override;

	virtual bool MatchesAndHasSameState(
		const FRootMotionSource* Other
	) const override;

	virtual bool UpdateStateFrom(
		const FRootMotionSource* SourceToTakeStateFrom,
		bool bMarkForSimulatedCatchup = false
	) override;

	virtual void PrepareRootMotion(
		float SimulationTime,
		float MovementTickTime,
		const ACharacter& Character,
		const UCharacterMovementComponent& MoveComponent
	) override;

	virtual void CheckTimeOut() override;

	UPROPERTY()
	TWeakObjectPtr<ATractionPoint> TractionPoinAcotrPtr;

	UPROPERTY()
	float InnerRadius = 100.f;
};

template <>
struct TStructOpsTypeTraits<FRootMotionSource_MyRadialForce> :
	public TStructOpsTypeTraitsBase2<FRootMotionSource_MyRadialForce>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};

USTRUCT()
struct FRootMotionSource_HasBeenFlyAway : public FRootMotionSource
{
	GENERATED_USTRUCT_BODY()

	FRootMotionSource_HasBeenFlyAway();

	virtual ~FRootMotionSource_HasBeenFlyAway() override;

	virtual FRootMotionSource* Clone() const override;

	virtual bool Matches(
		const FRootMotionSource* Other
	) const override;

	virtual bool MatchesAndHasSameState(
		const FRootMotionSource* Other
	) const override;

	virtual bool UpdateStateFrom(
		const FRootMotionSource* SourceToTakeStateFrom,
		bool bMarkForSimulatedCatchup = false
	) override;

	virtual void PrepareRootMotion(
		float SimulationTime,
		float MovementTickTime,
		const ACharacter& Character,
		const UCharacterMovementComponent& MoveComponent
	) override;

	virtual bool NetSerialize(
		FArchive& Ar,
		UPackageMap* Map,
		bool& bOutSuccess
	) override;

	virtual UScriptStruct* GetScriptStruct() const override;

	// 高度
	int32 Height = 100;
};

template <>
struct TStructOpsTypeTraits<FRootMotionSource_HasBeenFlyAway> :
	public TStructOpsTypeTraitsBase2<FRootMotionSource_HasBeenFlyAway>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};

USTRUCT()
struct FRootMotionSource_FlyAway : public FRootMotionSource
{
	GENERATED_USTRUCT_BODY()

	FRootMotionSource_FlyAway();

	virtual ~FRootMotionSource_FlyAway() override;

	virtual FRootMotionSource* Clone() const override;

	virtual bool Matches(
		const FRootMotionSource* Other
	) const override;

	virtual bool MatchesAndHasSameState(
		const FRootMotionSource* Other
	) const override;

	virtual bool UpdateStateFrom(
		const FRootMotionSource* SourceToTakeStateFrom,
		bool bMarkForSimulatedCatchup = false
	) override;

	virtual void PrepareRootMotion(
		float SimulationTime,
		float MovementTickTime,
		const ACharacter& Character,
		const UCharacterMovementComponent& MoveComponent
	) override;

	virtual bool NetSerialize(
		FArchive& Ar,
		UPackageMap* Map,
		bool& bOutSuccess
	) override;

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual void CheckTimeOut() override;

	void Initial(
		float Height,
		float Duration,
		int32 RisingSpeed,
		int32 FallingSpeed,
		const FVector& OriginalPt,
		ACharacter* CharacterPtr
	);

	void UpdateDuration(
		float Height,
		float Duration,
		int32 RisingSpeed,
		int32 FallingSpeed,
		const FVector& OriginalPt
	);

	// 上升速度 
	int32 RisingSpeed = 100;

	// 下降速度 < 0则使用默认的速度
	int32 FallingSpeed = -1;

	// 高度
	int32 Height = 100;

	bool bIsLanded = false;

	float Radius = 0.f;

	float HalfHeight = 0.f;

	int32 Line = 10000;
};

template <>
struct TStructOpsTypeTraits<FRootMotionSource_FlyAway> :
	public TStructOpsTypeTraitsBase2<FRootMotionSource_FlyAway>
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

	virtual bool Matches(
		const FRootMotionSource* Other
	) const override;

	virtual bool NetSerialize(
		FArchive& Ar,
		UPackageMap* Map,
		bool& bOutSuccess
	) override;

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual bool UpdateStateFrom(
		const FRootMotionSource* SourceToTakeStateFrom,
		bool bMarkForSimulatedCatchup = false
	) override;

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

template <>
struct TStructOpsTypeTraits<FRootMotionSource_BySpline> :
	public TStructOpsTypeTraitsBase2<FRootMotionSource_BySpline>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};

USTRUCT()
struct FRootMotionSource_Formation : public FRootMotionSource
{
	GENERATED_USTRUCT_BODY()

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual FRootMotionSource* Clone() const override;

	virtual bool Matches(
		const FRootMotionSource* Other
	) const override;

	virtual bool NetSerialize(
		FArchive& Ar,
		UPackageMap* Map,
		bool& bOutSuccess
	) override;

	virtual bool UpdateStateFrom(
		const FRootMotionSource* SourceToTakeStateFrom,
		bool bMarkForSimulatedCatchup = false
	) override;

	virtual void PrepareRootMotion(
		float SimulationTime,
		float MovementTickTime,
		const ACharacter& Character,
		const UCharacterMovementComponent& MoveComponent
	) override;

	USceneComponent* FormationPtr = nullptr;

private:
};

template <>
struct TStructOpsTypeTraits<FRootMotionSource_Formation> :
	public TStructOpsTypeTraitsBase2<FRootMotionSource_Formation>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};

/**
 * TODO 这个移动方式对NPC释放结束时的表现效果不正常，会被拉扯一下，原因未知
 */
USTRUCT()
struct FRootMotionSource_ByTornado : public FRootMotionSource
{
	GENERATED_USTRUCT_BODY()

	static FName RootMotionName;
	
	FRootMotionSource_ByTornado();

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual FRootMotionSource* Clone() const override;

	virtual bool NetSerialize(
		FArchive& Ar,
		UPackageMap* Map,
		bool& bOutSuccess
	) override;

	virtual bool Matches(
		const FRootMotionSource* Other
	) const override;

	virtual bool MatchesAndHasSameState(
		const FRootMotionSource* Other
	) const override;

	virtual bool UpdateStateFrom(
		const FRootMotionSource* SourceToTakeStateFrom,
		bool bMarkForSimulatedCatchup = false
	) override;

	virtual void PrepareRootMotion(
		float SimulationTime,
		float MovementTickTime,
		const ACharacter& Character,
		const UCharacterMovementComponent& MoveComponent
	) override;

	TWeakObjectPtr<ATornado> TornadoPtr = nullptr;

	float InnerRadius = 150.f;

	float MaxHeight = 300.f;

	float RotationSpeed = 360.f;
	
private:

	FVector CurrentRotatorDir = FVector::ZeroVector;
	
	float CurrentHeight = 0.f;

	float StopTime = .1f;
};

template <>
struct TStructOpsTypeTraits<FRootMotionSource_ByTornado> :
	public TStructOpsTypeTraitsBase2<FRootMotionSource_ByTornado>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};
