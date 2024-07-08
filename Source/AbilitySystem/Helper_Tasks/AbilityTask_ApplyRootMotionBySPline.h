// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotion_Base.h"
#include "GAOwnerTypeInterface.h"

#include "AbilityTask_ApplyRootMotionBySPline.generated.h"

class UCurveFloat;
class ASPlineActor;

DECLARE_DELEGATE(FOnTaskFinished);

/**
 *	Applies force to character's movement
 */
UCLASS()
class ABILITYSYSTEM_API UAbilityTask_ApplyRootMotionBySPline : public UAbilityTask_ApplyRootMotion_Base
{
	GENERATED_BODY()
public:

	using FOwnerInterfaceType = IGAOwnerInterface;

	using FOwnerType = ACharacter;

	static UAbilityTask_ApplyRootMotionBySPline* ApplyRootMotionBySpline
	(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		float Duration,
		ASPlineActor* InSPlineActorPtr,
		FOwnerType* InTargetCharacterPtr
	);

	virtual void Activate() override;

	virtual void TickTask(float DeltaTime) override;

	virtual void OnDestroy(bool AbilityIsEnding) override;

	FOnTaskFinished OnFinish;

protected:

	virtual void SharedInitAndApply() override;

protected:

	float Duration;

	ASPlineActor* SPlineActorPtr = nullptr;

	FOwnerType* TargetCharacterPtr = nullptr;

};
