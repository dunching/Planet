// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BasicFutures_Dash.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Kismet/KismetMathLibrary.h"

#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "AbilityTask_PlayMontage.h"
#include "AbilityTask_ARM_ConstantForce.h"
#include "AssetRefMap.h"
#include "AS_Character.h"
#include "GameplayTagsLibrary.h"
#include "ProxyProcessComponent.h"

#include "CharacterAbilitySystemComponent.h"
#include "HumanCharacter_Player.h"
#include "Planet_Tools.h"
#include "PlayerComponent.h"
#include "Tools.h"

static TAutoConsoleVariable<int32> SkillDrawDebugDash(
                                                      TEXT("Skill.DrawDebug.Dash"),
                                                      0,
                                                      TEXT("")
                                                      TEXT(" default: 0"));

UBasicFutures_Dash::UBasicFutures_Dash() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBasicFutures_Dash::PostCDOContruct()
{
	Super::PostCDOContruct();

	if (GetWorldImp())
	{
	}
}

void UBasicFutures_Dash::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
		CharacterPtr->LandedDelegate.AddDynamic(this, &ThisClass::OnLanded);
	}
}

void UBasicFutures_Dash::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	if (CharacterPtr)
	{
#if UE_EDITOR || UE_CLIENT
		if (
			(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
		)
		{
			if (CharacterPtr->IsA(AHumanCharacter_Player::StaticClass()))
			{
				auto PlayerCharacterPtr = Cast<AHumanCharacter_Player>(CharacterPtr);
				PlayerCharacterPtr->GetPlayerComponent()->SetCameraType(ECameraType::kDashing);
			}
		}
#endif
	}
}

void UBasicFutures_Dash::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());

	Start = CharacterPtr->GetActorLocation();

#ifdef WITH_EDITOR
	if (SkillDrawDebugDash.GetValueOnGameThread())
	{
		DrawDebugSphere(GetWorld(), Start, 20, 20, FColor::Red, false, 10);
	}
#endif

#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		CommitAbility(Handle, ActorInfo, ActivationInfo);
	}
#endif

	DoDash(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool UBasicFutures_Dash::CommitAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
)
{
	if (CharacterPtr)
	{
		if (CharacterPtr->GetCharacterMovement()->IsFlying() || CharacterPtr->GetCharacterMovement()->IsFalling())
		{
			DashInAir++;
		}
	}

	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void UBasicFutures_Dash::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	if (CharacterPtr)
	{
#if UE_EDITOR || UE_CLIENT
		if (
			(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
		)
		{
			if (CharacterPtr->IsA(AHumanCharacter_Player::StaticClass()))
			{
				auto PlayerCharacterPtr = Cast<AHumanCharacter_Player>(CharacterPtr);
				PlayerCharacterPtr->GetPlayerComponent()->SetCameraType(ECameraType::kAction);
			}
		}
#endif
	}

#ifdef WITH_EDITOR
	if (SkillDrawDebugDash.GetValueOnGameThread())
	{
		DrawDebugSphere(GetWorld(), CharacterPtr->GetActorLocation(), 20, 20, FColor::Red, false, 10);
		UE_LOG(LogTemp, Log, TEXT("%.2lf"), (CharacterPtr->GetActorLocation() - Start).Length());
	}
#endif

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UBasicFutures_Dash::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	OUT FGameplayTagContainer* OptionalRelevantTags
) const
{
	if (DashInAir >= MaxDashInAir)
	{
		return false;
	}

	if (CharacterPtr)
	{
		auto CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		if (CharacterAttributes->GetStamina() >= Consume)
		{
			return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
		}
	}

	return false;
}

void UBasicFutures_Dash::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);

	if (ActiveTasks.IsEmpty())
	{
		K2_CancelAbility();
	}
}

TMap<FGameplayTag, int32> UBasicFutures_Dash::GetCostMap() const
{
	return {
		TPair<FGameplayTag, int32>{
			UGameplayTagsLibrary::GEData_ModifyItem_Stamina, Consume
		}
	};
}

// void UBasicFutures_Dash::InitalDefaultTags()
// {
// 	Super::InitalDefaultTags();
//
// 	// // AbilityTags.AddTag(UGameplayTagsLibrary::Dash);
//
// 	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::State_ReleasingSkill);
//
// 	ActivationBlockedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_IntoFly);
//
// 	ActivationBlockedTags.AddTag(UGameplayTagsLibrary::State_RootMotion_FlyAway);
//
// 	ActivationBlockedTags.AddTag(UGameplayTagsLibrary::State_Debuff_Stun);
// 	ActivationBlockedTags.AddTag(UGameplayTagsLibrary::State_Debuff_Fear);
// 	ActivationBlockedTags.AddTag(UGameplayTagsLibrary::State_Debuff_Charm);
//
// 	// 在运动时不激活
// }

void UBasicFutures_Dash::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec
)
{
	if (CharacterPtr)
	{
		CharacterPtr->LandedDelegate.RemoveDynamic(this, &ThisClass::OnLanded);
	}
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UBasicFutures_Dash::DoDash(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (CharacterPtr)
	{
		if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
		{
			auto GameplayAbilityTargetData_DashPtr = dynamic_cast<const FGameplayAbilityTargetData_Dash*>(
				TriggerEventData->TargetData.Get(0));
			if (GameplayAbilityTargetData_DashPtr)
			{
				UAnimMontage* CurMontagePtr = nullptr;

				const FRotator Rotation = CharacterPtr->Controller->GetControlRotation();
				FVector Direction = FVector::ZeroVector;

				switch (GameplayAbilityTargetData_DashPtr->DashDirection)
				{
				case EDashDirection::kForward:
					{
						CurMontagePtr = ForwardMontage;
						Direction = UKismetMathLibrary::MakeRotFromZX(-CharacterPtr->GetGravityDirection(),
						                                              Rotation.Quaternion().GetForwardVector()).
							Vector();
					}
					break;
				case EDashDirection::kBackward:
					{
						CurMontagePtr = BackwardMontage;
						Direction = UKismetMathLibrary::MakeRotFromZX(-CharacterPtr->GetGravityDirection(),
						                                              -Rotation.Quaternion().GetForwardVector()).
							Vector();
					}
					break;
				case EDashDirection::kLeft:
					{
						CurMontagePtr = LeftMontage;
						Direction = UKismetMathLibrary::MakeRotFromZX(-CharacterPtr->GetGravityDirection(),
						                                              -Rotation.Quaternion().GetRightVector()).Vector();
					}
					break;
				case EDashDirection::kRight:
					{
						CurMontagePtr = RightMontage;
						Direction = UKismetMathLibrary::MakeRotFromZX(-CharacterPtr->GetGravityDirection(),
						                                              Rotation.Quaternion().GetRightVector()).Vector();
					}
					break;
				}

				const auto Rate = FMath::IsNearlyZero(Duration)
					                  ? CurMontagePtr->CalculateSequenceLength() / Duration
					                  : 1.f;
				PlayMontage(CurMontagePtr, Rate);

				Displacement(Direction);
			}
		}
	}
}

void UBasicFutures_Dash::PlayMontage(UAnimMontage* CurMontagePtr, float Rate)
{
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
	)
	{
		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			CurMontagePtr,
			Rate
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());

		TaskPtr->ReadyForActivation();
	}
}

void UBasicFutures_Dash::Displacement(const FVector& Direction)
{
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
	)
	{
		auto TaskPtr = UAbilityTask_ARM_ConstantForce::ApplyRootMotionConstantForce(
			this,
			TEXT(""),
			Direction,
			MoveLength / Duration,
			Duration,
			false,
			true,
			nullptr,
			ERootMotionFinishVelocityMode::SetVelocity,
			CharacterPtr->GetCharacterMovement()->Velocity,
			0.f,
			false
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());

		TaskPtr->ReadyForActivation();
	}
}

void UBasicFutures_Dash::OnLanded(const FHitResult&)
{
	DashInAir = 0;
}

UScriptStruct* FGameplayAbilityTargetData_Dash::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_Dash::StaticStruct();
}

bool FGameplayAbilityTargetData_Dash::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << DashDirection;

	return true;
}

void UBasicFutures_Dash::ApplyCostImp(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const TMap<FGameplayTag, int32>& CostMap
	) const
{
	auto AbilitySystemComponentPtr = Cast<UCharacterAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
	if (!AbilitySystemComponentPtr)
	{
		return;
	}

	UGameplayEffect* CostGE = GetCostGameplayEffect();
	if (CostGE)
	{
		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(CostGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::BaseFeature_Dash);
		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);

		const auto CostsMap = AbilitySystemComponentPtr->GetCost(CostMap);
		for (const auto& Iter : CostsMap)
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
														   Iter.Key,
														   -Iter.Value
														  );
		}
		const auto CDGEHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}
