
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

	CooldownTime = 10;
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
}

void USkill_Active_XYFH::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommitAbility(Handle, ActorInfo, ActivationInfo);

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

	PerformAction();
}

bool USkill_Active_XYFH::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return true;
	}
	return false;
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

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Active_XYFH::PerformAction()
{
	if (CharacterPtr && bIsContinue)
	{
		ExcuteTasks();
		PlayMontage();
	}
	else
	{
	}
}

void USkill_Active_XYFH::ExcuteTasks()
{
	auto HumanMontage = GetCurrentMontage();

	const auto SequenceLength = HumanMontage->CalculateSequenceLength();

	// 角色移动
	{
		auto TaskPtr = UAbilityTask_ApplyRootMotionBySPline::NewTask(
			this,
			TEXT(""),
			SequenceLength,
			SPlineActorPtr,
			CharacterPtr,
			StepIndex,
			StepIndex + 1
		);
		TaskPtr->OnFinish.BindUObject(this, &ThisClass::OnMoveStepComplete);
		TaskPtr->ReadyForActivation();
	}

	// 镜头控制
	{
		auto TaskPtr = UAbilityTask_ControlCameraBySpline::NewTask(
			this,
			TEXT(""),
			SequenceLength,
			CameraTrailHelperPtr,
			CharacterPtr,
			StepIndex,
			StepIndex + 1
		);
		TaskPtr->ReadyForActivation();
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
	StepIndex++;

	if (StepIndex >= MaxIndex)
	{
		K2_CancelAbility();
		return;
	}

	PerformAction();
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
