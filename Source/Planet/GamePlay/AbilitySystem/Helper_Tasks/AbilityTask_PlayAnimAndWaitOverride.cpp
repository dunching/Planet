// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilityTask_PlayAnimAndWaitOverride.h"

#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "AbilitySystemGlobals.h"

void UAbilityTask_PlayMontage::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	const bool bPlayingThisMontage = (Montage == MontageToPlay) && Ability && Ability->GetCurrentMontage() == MontageToPlay;
	if (bPlayingThisMontage)
	{
		// Reset AnimRootMotionTranslationScale
		ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
		if (Character && (Character->GetLocalRole() == ROLE_Authority ||
			(Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
		{
			Character->SetAnimRootMotionTranslationScale(1.f);
		}
	}

	if (bPlayingThisMontage && (bInterrupted || !bAllowInterruptAfterBlendOut))
	{
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (bInterrupted)
		{
			OnInterrupted.ExecuteIfBound();

			EndTask();
		}
		else
		{
			OnBlendOut.ExecuteIfBound();
		}
	}
}

void UAbilityTask_PlayMontage::OnGameplayAbilityCancelled()
{
	if (StopPlayingMontage() || bAllowInterruptAfterBlendOut)
	{
		// Let the BP handle the interrupt as well
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.ExecuteIfBound();
		}
	}

	EndTask();
}

void UAbilityTask_PlayMontage::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.ExecuteIfBound();
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.ExecuteIfBound();
		}
	}

	EndTask();
}

UAbilityTask_PlayMontage* UAbilityTask_PlayMontage::CreatePlayMontageAndWaitProxy(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	UAnimMontage* MontageToPlay,
	UAnimInstance* AnimInstancePtr,
	float Rate,
	FName StartSection,
	bool bStopWhenAbilityEnds,
	float AnimRootMotionTranslationScale,
	float StartTimeSeconds,
	bool bAllowInterruptAfterBlendOut
)
{
	UAbilityTask_PlayMontage* MyObj = NewAbilityTask<UAbilityTask_PlayMontage>(OwningAbility, TaskInstanceName);

	MyObj->Rate = Rate;
	MyObj->MontageToPlay = MontageToPlay;
	MyObj->AnimInstancePtr = AnimInstancePtr;
	MyObj->StartSection = StartSection;
	MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;
	MyObj->bAllowInterruptAfterBlendOut = bAllowInterruptAfterBlendOut;
	MyObj->StartTimeSeconds = StartTimeSeconds;

	return MyObj;
}

void UAbilityTask_PlayMontage::Activate()
{
	if (Ability == nullptr)
	{
		return;
	}

	bool bPlayedMontage = false;

	if (AnimInstancePtr != nullptr)
	{
		if (AnimInstancePtr->Montage_Play(MontageToPlay, Rate))
		{
			// Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if we are  pending kill.
			if (ShouldBroadcastAbilityTaskDelegates() == false)
			{
				return;
			}

			if (FAnimMontageInstance* MontageInstance = AnimInstancePtr->GetActiveInstanceForMontage(MontageToPlay))
			{
				MontageInstanceID = MontageInstance->GetInstanceID();
			}

			BlendingOutDelegate.BindUObject(this, &ThisClass::OnMontageBlendingOut);
			AnimInstancePtr->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

			MontageEndedDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
			AnimInstancePtr->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

			AnimInstancePtr->OnPlayMontageNotifyBegin.AddDynamic(this, &ThisClass::OnNotifyBeginReceived);
			AnimInstancePtr->OnPlayMontageNotifyEnd.AddDynamic(this, &ThisClass::OnNotifyEndReceived);

			ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
			if (Character && (Character->GetLocalRole() == ROLE_Authority ||
				(Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
			{
				Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
			}

			bPlayedMontage = true;
		}
	}

	if (!bPlayedMontage)
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontage called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay), *InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.ExecuteIfBound();
		}
	}
}

void UAbilityTask_PlayMontage::ExternalCancel()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCancelled.ExecuteIfBound();
	}
	Super::ExternalCancel();
}

bool UAbilityTask_PlayMontage::IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const
{
	return ((MontageInstanceID != INDEX_NONE) && (BranchingPointNotifyPayload.MontageInstanceID == MontageInstanceID));
}

void UAbilityTask_PlayMontage::OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
	{
		OnNotifyBegin.ExecuteIfBound(NotifyName);
	}
}

void UAbilityTask_PlayMontage::OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
	{
		OnNotifyEnd.ExecuteIfBound(NotifyName);
	}
}

void UAbilityTask_PlayMontage::OnDestroy(bool AbilityEnded)
{
	// Note: Clearing montage end delegate isn't necessary since its not a multicast and will be cleared when the next montage plays.
	// (If we are destroyed, it will detect this and not do anything)

	// This delegate, however, should be cleared as it is a multicast
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(InterruptedHandle);
		if (AbilityEnded && bStopWhenAbilityEnds)
		{
			StopPlayingMontage();
		}
	}

	Super::OnDestroy(AbilityEnded);

}

bool UAbilityTask_PlayMontage::StopPlayingMontage()
{
	if (Ability == nullptr)
	{
		return false;
	}

	if (AnimInstancePtr)
	{
		AnimInstancePtr->Montage_Stop(BlendOutTime, MontageToPlay);

		FAnimMontageInstance* MontageInstance = AnimInstancePtr->GetActiveInstanceForMontage(MontageToPlay);
		if (MontageInstance)
		{
			MontageInstance->OnMontageBlendingOutStarted.Unbind();
			MontageInstance->OnMontageEnded.Unbind();
		}
	}

	return false;
}
