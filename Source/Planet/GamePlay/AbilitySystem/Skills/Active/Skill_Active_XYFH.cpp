
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
#include "Net/UnrealNetwork.h"


#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
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
#include "GameplayTagsLibrary.h"
#include "CharacterAbilitySystemComponent.h"
#include "CameraTrailHelper.h"
#include "AbilityTask_ControlCameraBySpline.h"

USkill_Active_XYFH::USkill_Active_XYFH() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

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

//	TargetOffsetValue.SetValue(CharacterPtr->GetCameraBoom()->TargetOffset);

	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
		)
	{
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
	}
}

void USkill_Active_XYFH::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
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
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
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
	}

	StepIndex = 0;
	SubStepIndex = 0;

//	TargetOffsetValue.ReStore();

#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode()  == NM_DedicatedServer)
	{
		CommitAbility(Handle, ActorInfo, ActivationInfo);
	}
#endif

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Active_XYFH::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, SPlineActorPtr, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, StepIndex, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ThisClass, SubStepIndex, COND_SkipOwner);
}

void USkill_Active_XYFH::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (CharacterPtr)
	{
		switch (StepIndex)
		{
		case 0:
		{
			PlayMontage();

			auto HumanMontage = GetCurrentMontage();

			const auto Duration = HumanMontage->CalculateSequenceLength();

			ExcuteTasks(StepIndex, -1.f, StepIndex + 1, -1.f, Duration, false);
		}
		break;
		case 1:
		case 2:
		case 3:
		{
			switch (SubStepIndex)
			{
			case 0:
			{
				CharacterPtr->GetMesh()->SetHiddenInGame(true);

				ExcuteTasks(StepIndex,-1.f, StepIndex + 1, .8f, SubStepMoveDuration, true);
			}
			break;
			case 1:
			{
				PlayMontage();

				auto HumanMontage = GetCurrentMontage();

				const auto Duration = HumanMontage->CalculateSequenceLength();

				ExcuteTasks(StepIndex, .8f, StepIndex + 1, -1.f, Duration, false);
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

void USkill_Active_XYFH::ExcuteTasks(
	int32 StartPtIndex,
	float StartOffset,
	int32 EndPtIndex,
	float EndOffset,
	float Duration,
	bool bIsSubMoveStep
)
{
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
		)
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

		// 角色移动
		auto TaskPtr = UAbilityTask_ApplyRootMotionBySPline::NewTask(
			this,
			TEXT(""),
			Duration,
			SPlineActorPtr,
			CharacterPtr,
			StartPtIndex,
			StartOffset,
			EndPtIndex,
			EndOffset
		);
		if (bIsSubMoveStep)
		{
			TaskPtr->OnFinish.BindUObject(this, &ThisClass::OnSubMoveStepComplete);
		}
		else
		{
			TaskPtr->OnFinish.BindUObject(this, &ThisClass::OnMoveStepComplete);
		}
		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());

		TaskPtr->ReadyForActivation();
	}
}

void USkill_Active_XYFH::PlayMontage()
{
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
		)
	{
		const float InPlayRate = 1.f;

		auto HumanMontage = GetCurrentMontage();

		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			HumanMontage,
			InPlayRate
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());
		TaskPtr->OnCompleted.BindUObject(this, &ThisClass::OnPlayMontageEnd);
		TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::OnPlayMontageEnd);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());

		TaskPtr->ReadyForActivation();
	}
}

void USkill_Active_XYFH::OnPlayMontageEnd()
{
}

void USkill_Active_XYFH::OnMoveStepComplete()
{
#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode()  == NM_DedicatedServer)
	{
		if (StepIndex >= MaxIndex)
		{
			K2_CancelAbility();
			return;
		}
	}
#endif

	CheckInContinue();
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
