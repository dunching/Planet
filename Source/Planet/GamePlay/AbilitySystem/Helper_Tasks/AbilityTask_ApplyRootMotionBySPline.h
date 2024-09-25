// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotion_Base.h"

#include "AbilityTask_ApplyRootMotionBySPline.generated.h"

class UCurveFloat;
class ASPlineActor;
class ACharacterBase;

DECLARE_DELEGATE(FOnTaskFinished);

/**
 *
 */
UCLASS()
class PLANET_API UAbilityTask_ApplyRootMotionBySPline : public UAbilityTask_ApplyRootMotion_Base
{
	GENERATED_BODY()
public:

	static UAbilityTask_ApplyRootMotionBySPline* NewTask
	(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		float Duration,
		ASPlineActor* InSPlineActorPtr,
		ACharacterBase* InTargetCharacterPtr,
		int32 StartPtIndex,
		float StartOffset,
		int32 EndPtIndex,
		float EndOffset
	);

	static UAbilityTask_ApplyRootMotionBySPline* NewTask
	(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		float Duration,
		ASPlineActor * InSPlineActorPtr,
		ACharacterBase* InTargetCharacterPtr
	);

	virtual void Activate() override;

	virtual void TickTask(float DeltaTime) override;

	virtual void OnDestroy(bool AbilityIsEnding) override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	FOnTaskFinished OnFinish;

protected:

	virtual void SharedInitAndApply() override;

protected:

	UPROPERTY(Replicated)
	float StartDistance = 0.f;

	UPROPERTY(Replicated)
	float EndDistance = 0.f;

	UPROPERTY(Replicated)
	float Duration = 1.f;

	UPROPERTY(Replicated)
	ASPlineActor* SPlineActorPtr = nullptr;

	UPROPERTY(Replicated)
	ACharacterBase* TargetCharacterPtr = nullptr;

};
