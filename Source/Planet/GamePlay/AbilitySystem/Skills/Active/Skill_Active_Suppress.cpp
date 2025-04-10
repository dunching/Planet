
#include "Skill_Active_Suppress.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "KismetCollisionHelper.h"
#include "KismetGravityLibrary.h"


#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "Tool_PickAxe.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "Planet.h"
#include "CollisionDataStruct.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "CharacterAbilitySystemComponent.h"
#include "GameplayTagsLibrary.h"
#include "CS_RootMotion.h"
#include "CS_PeriodicStateModify_Suppress.h"
#include "BasicFutures_MoveToAttaclArea.h"
#include "AbilityTask_ARM_MoveToForce.h"

static TAutoConsoleVariable<int32> Skill_Active_Suppress_Debug(
	TEXT("Skill_Active_Suppress.Debug"),
	0,
	TEXT("")
	TEXT(" default: 0"));

bool USkill_Active_Suppress::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	if (
		CharacterPtr->GetCharacterMovement()->IsFlying() ||
		CharacterPtr->GetCharacterMovement()->IsFalling()
		)
	{
		return false;
	}
#if UE_EDITOR || UE_CLIENT
	if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() < ROLE_Authority)
	{
		if (HasFocusActor())
		{
			const auto bIsHaveTargetInDistance = CheckTargetInDistance(MaxDistance);
			if (bIsHaveTargetInDistance)
			{
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
#endif
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USkill_Active_Suppress::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	if (MoveCompletedSignatureHandle)
	{
		MoveCompletedSignatureHandle->UnBindCallback();
		MoveCompletedSignatureHandle = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Active_Suppress::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (CharacterPtr)
	{
#if UE_EDITOR || UE_SERVER
		if (GetAbilitySystemComponentFromActorInfo()->GetNetMode()  == NM_DedicatedServer)
		{
			CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());
		}
#endif

#if UE_EDITOR || UE_CLIENT
		if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() < ROLE_Authority)
		{
			const auto bIsHaveTargetInDistance = CheckTargetIsEqualDistance(Distance);
			if (bIsHaveTargetInDistance)
			{
				PerformAction_Server();
				PerformActionImp();
			}
			else
			{
				auto FocusCharacterPtr = HasFocusActor();

				const auto FocusCharacterPt = FocusCharacterPtr->GetActorLocation();

				const auto StartPt = CharacterPtr->GetActorLocation();
				const auto TargetPt = FocusCharacterPt + ((StartPt - FocusCharacterPt).GetSafeNormal() * Distance);

#ifdef WITH_EDITOR
				if (Skill_Active_Suppress_Debug.GetValueOnGameThread())
				{
					DrawDebugSphere(GetWorld(), TargetPt, 10, 24, FColor::Red, false, 10);
				}
#endif

				PerformMove_Server(StartPt, TargetPt);
				PerformMoveImp(StartPt, TargetPt);
			}
		}
#endif
	}
}

void USkill_Active_Suppress::PerformAction_Server_Implementation()
{
	PerformActionImp();
}

void USkill_Active_Suppress::PerformActionImp()
{
	ExcuteTasks();
	PlayMontage();
}

void USkill_Active_Suppress::PerformMove_Server_Implementation(const FVector& StartPt, const FVector& TargetPt)
{
	PerformMoveImp(StartPt, TargetPt);
}

void USkill_Active_Suppress::PerformMoveImp(const FVector& StartPt, const FVector& TargetPt)
{
	auto TaskPtr = UAbilityTask_ARM_MoveToForce::ApplyRootMotionMoveToForce(
		this,
		TEXT(""),
		StartPt,
		TargetPt,
		Duration
	);

	TaskPtr->Ability = this;
	TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());
	TaskPtr->OnFinished.BindUObject(this, &ThisClass::MoveCompletedSignature);

	TaskPtr->ReadyForActivation();
}

void USkill_Active_Suppress::ExcuteTasks()
{
	if (CharacterPtr)
	{
#if UE_EDITOR || UE_SERVER
		if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
		{
			auto TargetCharacter = HasFocusActor();
			ExcuteTasksImp(TargetCharacter);
		}
#endif
	}
}

void USkill_Active_Suppress::ExcuteTasksImp_Implementation(ACharacterBase* TargetCharacterPtr)
{
	if (CharacterPtr)
	{
#if UE_EDITOR || UE_SERVER
		if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
		{
			// 伤害
			auto ICPtr = CharacterPtr->GetCharacterAbilitySystemComponent();

		}
#endif
	}
}

void USkill_Active_Suppress::PlayMontage()
{
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
		)
	{
		const float InPlayRate = 1.f;

		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			HumanMontage,
			InPlayRate
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());
		TaskPtr->OnCompleted.BindUObject(this, &ThisClass::K2_CancelAbility);
		TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::K2_CancelAbility);

		TaskPtr->ReadyForActivation();
	}
}

void USkill_Active_Suppress::MoveCompletedSignature()
{
#if UE_EDITOR || UE_CLIENT
	if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() < ROLE_Authority)
	{
		const auto bIsHaveTargetInDistance = CheckTargetIsEqualDistance(Distance);
		if (bIsHaveTargetInDistance)
		{
			PerformAction_Server();
			PerformActionImp();
		}
		else
		{
			// 未能达到需要的距离
			CancelAbility_Server();
		}
	}
#endif
}
