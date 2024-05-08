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

class ASPlineActor;
class ACharacterBase;

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
