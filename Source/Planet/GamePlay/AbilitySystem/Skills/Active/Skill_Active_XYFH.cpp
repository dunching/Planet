
#include "Skill_Active_XYFH.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"
#include "Components/SplineComponent.h"
#include <Components/CapsuleComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "Kismet/KismetMathLibrary.h"
#include <Engine/OverlapResult.h>
#include <GameFramework/SpringArmComponent.h>

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "Tool_PickAxe.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "Planet.h"
#include "CollisionDataStruct.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "AbilityTask_TimerHelper.h"
#include "Helper_RootMotionSource.h"
#include "AbilityTask_tornado.h"
#include "CS_RootMotion.h"
#include "GameplayTagsSubSystem.h"
#include "InteractiveBaseGAComponent.h"
#include "CameraTrailHelper.h"
#include "AbilityTask_ControlCameraBySpline.h"

USkill_Active_XYFH::USkill_Active_XYFH() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bRetriggerInstancedAbility = true;

	CurrentWaitInputTime = 1.f;
}

void USkill_Active_XYFH::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	TargetOffsetValue.SetValue(CharacterPtr->GetCameraBoom()->TargetOffset);
}

void USkill_Active_XYFH::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!SPlineActorPtr)
	{
		SPlineActorPtr = GetWorld()->SpawnActor<ASPlineActor>(
			SPlineActorClass, CharacterPtr->GetActorTransform()
		);
	}

	if (!CameraTrailHelperPtr)
	{
		CameraTrailHelperPtr = GetWorld()->SpawnActor<ACameraTrailHelper>(
			CameraTrailHelperClass, CharacterPtr->GetActorTransform()
		);
	}

	PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool USkill_Active_XYFH::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USkill_Active_XYFH::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	if (SPlineActorPtr)
	{
		SPlineActorPtr->Destroy();
		SPlineActorPtr = nullptr;
	}

	if (CameraTrailHelperPtr)
	{
		CameraTrailHelperPtr->Destroy();
		CameraTrailHelperPtr = nullptr;
	}

	StepIndex = 0;
	SubStepIndex = 0;

	TargetOffsetValue.ReStore();

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Active_XYFH::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (CharacterPtr && bIsContinue)
	{
		switch (StepIndex)
		{
		case 0:
		{
			const auto StartDistance =
				SPlineActorPtr->SplineComponentPtr->GetDistanceAlongSplineAtSplinePoint(StepIndex);
			const auto EndDistance =
				SPlineActorPtr->SplineComponentPtr->GetDistanceAlongSplineAtSplinePoint(StepIndex + 1);

			PlayMontage();

			auto HumanMontage = GetCurrentMontage();

			const auto Duration = HumanMontage->CalculateSequenceLength();

			ExcuteTasks(StartDistance, EndDistance, Duration, false);
		}
		break;
		case 1:
		case 2:
		case 3:
		{
			const auto StartDistance =
				SPlineActorPtr->SplineComponentPtr->GetDistanceAlongSplineAtSplinePoint(StepIndex);
			const auto EndDistance =
				SPlineActorPtr->SplineComponentPtr->GetDistanceAlongSplineAtSplinePoint(StepIndex + 1);

			const auto Offset = (EndDistance - StartDistance) * 0.8f;

			switch (SubStepIndex)
			{
			case 0:
			{
				CharacterPtr->GetMesh()->SetHiddenInGame(true);

				ExcuteTasks(StartDistance, StartDistance + Offset, SubStepMoveDuration, true);
			}
			break;
			case 1:
			{
				PlayMontage();

				auto HumanMontage = GetCurrentMontage();

				const auto Duration = HumanMontage->CalculateSequenceLength();

				ExcuteTasks(StartDistance + Offset, EndDistance, Duration, false);
			}
			break;
			}
		}
		break;
		default:
			break;
		}
	}
	else
	{
	}
}

void USkill_Active_XYFH::ExcuteTasks(float StartDistance, float EndDistance, float Duration, bool bIsSubMoveStep)
{
	if (bIsSubMoveStep)
	{
		SubStepIndex++;
	}
	else
	{
		StepIndex++;
		SubStepIndex = 0;
	}

	// ��ɫ�ƶ�
	{
		auto TaskPtr = UAbilityTask_ApplyRootMotionBySPline::NewTask(
			this,
			TEXT(""),
			Duration,
			SPlineActorPtr,
			CharacterPtr,
			StartDistance,
			EndDistance
		);
		if (bIsSubMoveStep)
		{
			TaskPtr->OnFinish.BindUObject(this, &ThisClass::OnSubMoveStepComplete);
		}
		else
		{
			TaskPtr->OnFinish.BindUObject(this, &ThisClass::OnMoveStepComplete);
		}
		TaskPtr->ReadyForActivation();
	}

	// ��ͷ����
	{
		auto TaskPtr = UAbilityTask_ControlCameraBySpline::NewTask(
			this,
			TEXT(""),
			Duration,
			CameraTrailHelperPtr,
			CharacterPtr,
			StartDistance,
			EndDistance
		);
//		TaskPtr->ReadyForActivation();
	}
}

void USkill_Active_XYFH::PlayMontage()
{
	{
		const float InPlayRate = 1.f;

		auto HumanMontage = GetCurrentMontage();

		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			HumanMontage,
			CharacterPtr->GetMesh()->GetAnimInstance(),
			InPlayRate
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());
		TaskPtr->OnCompleted.BindUObject(this, &ThisClass::OnPlayMontageEnd);
		TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::OnPlayMontageEnd);

		TaskPtr->ReadyForActivation();
	}
}

void USkill_Active_XYFH::OnPlayMontageEnd()
{
}

void USkill_Active_XYFH::OnMoveStepComplete()
{
	if (StepIndex >= MaxIndex)
	{
		K2_CancelAbility();
		return;
	}

	WaitInput();
}

void USkill_Active_XYFH::OnSubMoveStepComplete()
{
	CharacterPtr->GetMesh()->SetHiddenInGame(false);

	PerformAction(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), &CurrentEventData);
}

UAnimMontage* USkill_Active_XYFH::GetCurrentMontage() const
{
	UAnimMontage* HumanMontage = nullptr;

	switch (StepIndex)
	{
	case 0:
	{
		HumanMontage = HumanMontage1;
	}
	break;
	case 1:
	{
		HumanMontage = HumanMontage2;
	}
	break;
	case 2:
	{
		HumanMontage = HumanMontage3;
	}
	break;
	case 3:
	{
		HumanMontage = HumanMontage4;
	}
	break;
	default:
		break;
	}

	return HumanMontage;
}