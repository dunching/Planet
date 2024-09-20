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
 *	Applies force to character's movement
 */
UCLASS()
class PLANET_API UAbilityTask_ApplyRootMotionBySPline : public UAbilityTask_ApplyRootMotion_Base
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_ApplyRootMotionBySPline* NewTask
	(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		float Duration,
		ASPlineActor* InSPlineActorPtr,
		ACharacterBase* InTargetCharacterPtr,
		int32 StartPtIndex = 0,
		int32 EndPtIndex = -1
	);

	static UAbilityTask_ApplyRootMotionBySPline* NewTask
	(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		float Duration,
		ASPlineActor* InSPlineActorPtr,
		ACharacterBase* InTargetCharacterPtr,
		float StartDistance,
		float EndDistance
	);

	virtual void Activate() override;

	virtual void TickTask(float DeltaTime) override;

	virtual void OnDestroy(bool AbilityIsEnding) override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	FOnTaskFinished OnFinish;

protected:

	virtual void SharedInitAndApply() override;

protected:
	
	float StartDistance = 0.f;
	
	float EndDistance = 0.f;
	
	float Duration = 1.f;
	
	ASPlineActor* SPlineActorPtr = nullptr;
	
	ACharacterBase* TargetCharacterPtr = nullptr;

};
