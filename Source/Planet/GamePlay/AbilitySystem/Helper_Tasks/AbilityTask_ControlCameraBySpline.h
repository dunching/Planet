// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Abilities/Tasks/AbilityTask.h"

#include "AbilityTask_ControlCameraBySpline.generated.h"

class UCurveFloat;
class ACameraTrailHelper;
class ACharacterBase;

DECLARE_DELEGATE(FOnTaskFinished);

/**
 *	Applies force to character's movement
 */
UCLASS()
class PLANET_API UAbilityTask_ControlCameraBySpline : public UAbilityTask
{
	GENERATED_BODY()
public:

	UAbilityTask_ControlCameraBySpline(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_ControlCameraBySpline* NewTask
	(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		float Duration,
		ACameraTrailHelper* CameraTrailHelperPtr,
		ACharacterBase* InTargetCharacterPtr,
		int32 StartPtIndex = 0,
		int32 EndPtIndex = -1
	);
	static UAbilityTask_ControlCameraBySpline* NewTask
	(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		float Duration,
		ACameraTrailHelper* CameraTrailHelperPtr,
		ACharacterBase* InTargetCharacterPtr,
		float StartDistance,
		float EndDistance
	);

	virtual void Activate() override;

	virtual void TickTask(float DeltaTime) override;

	virtual void OnDestroy(bool AbilityIsEnding) override;

	FOnTaskFinished OnFinish;

protected:

protected:

	float StartDistance = 0.f;

	float EndDistance = 0.f;

	float StartTime = 0.f;

	float EndTime = 0.f;

	float CurrentTime = 0.f;

	float Duration = 1.f;

	ACameraTrailHelper* CameraTrailHelperPtr = nullptr;

	ACharacterBase* TargetCharacterPtr = nullptr;

};
