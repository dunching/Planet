// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "HAL/IConsoleManager.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Class.h"
#include "Engine/NetSerialization.h"
#include "Animation/AnimationAsset.h"
#include "GameFramework/RootMotionSource.h"
#include "GAOwnerTypeInterface.h"

#include "Helper_RootMotionSource.generated.h"

class ASPlineActor;
class ATornado;
class ACharacter;

USTRUCT()
struct FRootMotionSource_BySpline : public FRootMotionSource
{
	GENERATED_USTRUCT_BODY()

	using FOwnerInterfaceType = IGAOwnerInterface;

	using FOwnerType = ACharacter;

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

	FOwnerType* TargetCharacterPtr = nullptr;
};

USTRUCT()
struct FRootMotionSource_ByTornado : public FRootMotionSource
{
	GENERATED_USTRUCT_BODY()

	using FOwnerInterfaceType = IGAOwnerInterface;

	using FOwnerType = ACharacter;

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
	
	// ������ֵ��С���ᵼ���ƶ�ʱ��������Ӷ����Ա����ƶ�
	float OuterRadius = 150.f;
	
	float MaxHeight = 200.f;

	float RotationSpeed = 360.f;

	ATornado* TornadoPtr = nullptr;

	ACharacter* TargetCharacterPtr = nullptr;
};
