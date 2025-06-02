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
#include "PlanetModule.h"
#include "CollisionDataStruct.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "CharacterAbilitySystemComponent.h"
#include "GameplayTagsLibrary.h"
#include "BasicFutures_MoveToAttaclArea.h"
#include "AbilityTask_ARM_MoveToForce.h"
#include "AssetRefMap.h"
#include "ItemProxy_Skills.h"
#include "StateProcessorComponent.h"

static TAutoConsoleVariable<int32> Skill_Active_Suppress_Debug(
                                                               TEXT("Skill_Active_Suppress.Debug"),
                                                               0,
                                                               TEXT("")
                                                               TEXT(" default: 0")
                                                              );

void USkill_Active_Suppress::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
	)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (SkillProxyPtr)
	{
		ItemProxy_DescriptionPtr = Cast<FItemProxy_DescriptionType>(
		                                                            DynamicCastSharedPtr<FActiveSkillProxy>(
			                                                             SkillProxyPtr
			                                                            )->GetTableRowProxy_ActiveSkillExtendInfo()
		                                                           );
	}
}

bool USkill_Active_Suppress::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
	) const
{
	if (GetTargetInDistance(ItemProxy_DescriptionPtr->MaxDistance))
	{
	}
	else
	{
		return false;
	}

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
	CharacterPtr->GetStateProcessorComponent()->RemoveGetOrientFunc(
	                                                                GetOrientPrority
	                                                               );

	if (MoveCompletedSignatureHandle)
	{
		MoveCompletedSignatureHandle->UnBindCallback();
		MoveCompletedSignatureHandle = nullptr;
	}

#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveActiveGameplayEffect(SuppressGEHandle);
	}
#endif

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
		if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
		{
			CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());

			auto SpecHandle =
				MakeOutgoingGameplayEffectSpec(UAssetRefMap::GetInstance()->SuppressClass, GetAbilityLevel());
			SpecHandle.Data.Get()->AddDynamicAssetTag(SkillProxyPtr->GetProxyType());

			SuppressGEHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
		}
#endif

#if UE_EDITOR || UE_CLIENT
		if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
		{
		}
#endif

		auto FocusCharacterPtr = GetTargetInDistance(ItemProxy_DescriptionPtr->MaxDistance);

		if (FocusCharacterPtr)
		{
			TargetCharacterPtr = FocusCharacterPtr;
			const auto FocusCharacterPt = FocusCharacterPtr->GetActorLocation();

			const auto StartPt = CharacterPtr->GetActorLocation();
			const auto EndPt = FocusCharacterPt + (
				                   (StartPt - FocusCharacterPt).GetSafeNormal() * ItemProxy_DescriptionPtr->Distance);

#ifdef WITH_EDITOR
			if (Skill_Active_Suppress_Debug.GetValueOnGameThread())
			{
				DrawDebugSphere(GetWorld(), EndPt, 10, 24, FColor::Red, false, 10);
			}
#endif

			CharacterPtr->GetStateProcessorComponent()->AddGetOrientFunc(
			                                                             GetOrientPrority,
			                                                             std::bind(
				                                                              &ThisClass::GetOrient,
				                                                              this,
				                                                              std::placeholders::_1,
				                                                              std::placeholders::_2
				                                                             )
			                                                            );

			PerformMoveImp(StartPt, EndPt);

			return;
		}
	}

	K2_CancelAbility();
}

void USkill_Active_Suppress::PerformActionImp()
{
	ExcuteTasks();
	PlayMontage();
}

void USkill_Active_Suppress::PerformMoveImp(
	const FVector& StartPt,
	const FVector& TargetPt
	)
{
	TargetLocation = TargetPt;
	auto TaskPtr = UAbilityTask_ARM_MoveToForce::ApplyRootMotionMoveToForce(
	                                                                        this,
	                                                                        TEXT(""),
	                                                                        StartPt,
	                                                                        TargetPt,
	                                                                        ItemProxy_DescriptionPtr->MoveDuration
	                                                                       );

	TaskPtr->Ability = this;
	TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());
	TaskPtr->OnFinished.BindUObject(this, &ThisClass::MoveCompletedSignature);

	TaskPtr->ReadyForActivation();
}

void USkill_Active_Suppress::ExcuteTasks()
{
#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
	{
		if (CharacterPtr && TargetCharacterPtr)
		{
			TargetCharacterPtr->GetCharacterAbilitySystemComponent()->HasbeenSuppress(
				 CharacterPtr,
				 ItemProxy_DescriptionPtr->TargetMontage,
				 ItemProxy_DescriptionPtr->HumanMontage->GetPlayLength()
				);
		}
	}
#endif
}

void USkill_Active_Suppress::PlayMontage()
{
	const float InPlayRate = 1.f;
	{
		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			 this,
			 TEXT(""),
			 ItemProxy_DescriptionPtr->HumanMontage,
			 InPlayRate
			);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());

		if (
			(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
		)
		{
			TaskPtr->OnCompleted.BindUObject(this, &ThisClass::K2_CancelAbility);
			TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::K2_CancelAbility);
		}
		if (
			(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
			(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
		)
		{
			TaskPtr->ReadyForActivation();
		}
	}
}

void USkill_Active_Suppress::MoveCompletedSignature()
{
	const auto CurrentDistance = FVector::Dist2D(CharacterPtr->GetActorLocation(), TargetLocation);
	if (CurrentDistance <= 10.f)
	{
		PerformActionImp();
	}
	else
	{
#if UE_EDITOR || UE_SERVER
		if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
		{
			// 未能达到需要的距离
			K2_CancelAbility();
		}
#endif
	}
}

bool USkill_Active_Suppress::GetOrient(
	FRotator& DesiredRotation,
	bool& bIsImmediatelyRot
	)
{
	if (CharacterPtr && TargetCharacterPtr)
	{
		const auto CurrentLocation = CharacterPtr->GetActorLocation();
		// Normalized
		const auto Z = -UKismetGravityLibrary::GetGravity();
		DesiredRotation = UKismetMathLibrary::MakeRotFromZX(
		                                                    Z,
		                                                    TargetCharacterPtr->GetActorLocation() - CurrentLocation
		                                                   );
		bIsImmediatelyRot = true;
		return true;
	}

	return false;
}
