// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Animation/AnimInstance.h"
#include "Abilities/Tasks/AbilityTask.h"

#include "AbilityTask_PlayMontage.generated.h"

DECLARE_DELEGATE(FMontageWaitSimpleDelegate);

DECLARE_DELEGATE_OneParam(FMontageNotifyDelegate, FName);

UCLASS()
class PLANET_API UAbilityTask_PlayMontage : public UAbilityTask
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FMontageWaitSimpleDelegate	OnCompleted;

	UPROPERTY(BlueprintAssignable)
	FMontageWaitSimpleDelegate	OnBlendOut;

	UPROPERTY(BlueprintAssignable)
	FMontageWaitSimpleDelegate	OnInterrupted;

	UPROPERTY(BlueprintAssignable)
	FMontageWaitSimpleDelegate	OnCancelled;

	FMontageNotifyDelegate	OnNotifyBegin;

	FMontageNotifyDelegate	OnNotifyEnd;

	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	/** Callback function for when the owning Gameplay Ability is cancelled */
	UFUNCTION()
	void OnGameplayAbilityCancelled();

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks")
	static UAbilityTask_PlayMontage* CreatePlayMontageAndWaitProxy(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName, 
		UAnimMontage* MontageToPlay,
		UAnimInstance* AnimInstancePtr,
		float Rate = 1.f,
		FName StartSection = NAME_None, 
		bool bStopWhenAbilityEnds = true, 
		float AnimRootMotionTranslationScale = 1.f, 
		float StartTimeSeconds = 0.f,
		bool bAllowInterruptAfterBlendOut = false
	);

	/** Called when the ability is asked to cancel from an outside node. What this means depends on the individual task. By default, this does nothing other than ending the task. */
	virtual void ExternalCancel() override;

protected:

	virtual void Activate() override;

	bool IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const;

	UFUNCTION()
	void OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	UFUNCTION()
	void OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	virtual void OnDestroy(bool AbilityEnded) override;

	/** Checks if the ability is playing a montage and stops that montage, returns true if a montage was stopped, false if not. */
	bool StopPlayingMontage();

	FOnMontageBlendingOutStarted BlendingOutDelegate;

	FOnMontageEnded MontageEndedDelegate;

	FDelegateHandle InterruptedHandle;

	int32 MontageInstanceID;

	UPROPERTY()
	TObjectPtr<UAnimMontage> MontageToPlay;

	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstancePtr;

	UPROPERTY()
	float Rate;

	UPROPERTY()
	FName StartSection;

	UPROPERTY()
	float AnimRootMotionTranslationScale;

	UPROPERTY()
	float StartTimeSeconds;

	UPROPERTY()
	bool bStopWhenAbilityEnds;

	UPROPERTY()
	bool bAllowInterruptAfterBlendOut;
};
