// Copyright 2020 Dan Kestranek.

#include "PlanetGameplayAbility_Dash.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Kismet/KismetMathLibrary.h"

#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "AbilityTask_PlayMontage.h"
#include "AbilityTask_MyApplyRootMotionConstantForce.h"
#include "AssetRefMap.h"
#include "EquipmentElementComponent.h"
#include "GameplayTagsSubSystem.h"

UPlanetGameplayAbility_Dash::UPlanetGameplayAbility_Dash() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPlanetGameplayAbility_Dash::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
	}
}

void UPlanetGameplayAbility_Dash::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());

	Start = CharacterPtr->GetActorLocation();
	DrawDebugSphere(GetWorld(), Start, 20, 20, FColor::Red, false, 10);

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	DoDash(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UPlanetGameplayAbility_Dash::PreActivate(
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
		CharacterPtr->GetEquipmentItemsComponent()->AddTag(UGameplayTagsSubSystem::GetInstance()->DashAbilityTag);
	}
}

bool UPlanetGameplayAbility_Dash::CommitAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
)
{
	if (Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags))
	{
		if (CharacterPtr)
		{
			auto& CharacterAttributesRef = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
			if (CharacterAttributesRef.PP.GetCurrentValue() <= 0)
			{
			}
			else
			{
				CharacterAttributesRef.PP.AddCurrentValue(-Consume, CharacterAttributesRef.PropertuModify_GUID);
				return true;
			}
		}
	}

	return false;
}

void UPlanetGameplayAbility_Dash::EndAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	if (CharacterPtr)
	{
		CharacterPtr->GetEquipmentItemsComponent()->RemoveTag(UGameplayTagsSubSystem::GetInstance()->DashAbilityTag);
	}

	DrawDebugSphere(GetWorld(), CharacterPtr->GetActorLocation(), 20, 20, FColor::Red, false, 10);
	UE_LOG(LogTemp, Log, TEXT("%.2lf"), (CharacterPtr->GetActorLocation() - Start).Length());

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UPlanetGameplayAbility_Dash::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayTagContainer* SourceTags, 
	const FGameplayTagContainer* TargetTags, 
	OUT FGameplayTagContainer* OptionalRelevantTags
) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		if (CharacterPtr)
		{
			auto& PawnDataStructPtr = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
			if (PawnDataStructPtr.PP.GetCurrentValue() >= Consume)
			{
				return true;
			}
		}
	}

	return false;
}

void UPlanetGameplayAbility_Dash::DoDash(
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
			auto GameplayAbilityTargetData_DashPtr = dynamic_cast<const FGameplayAbilityTargetData_Dash*>(TriggerEventData->TargetData.Get(0));
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
					Direction = UKismetMathLibrary::MakeRotFromZX(-CharacterPtr->GetGravityDirection(), Rotation.Quaternion().GetForwardVector()).Vector();
				}
				break;
				case EDashDirection::kBackward:
				{
					CurMontagePtr = BackwardMontage;
					Direction = UKismetMathLibrary::MakeRotFromZX(-CharacterPtr->GetGravityDirection(), -Rotation.Quaternion().GetForwardVector()).Vector();
				}
				break;
				case EDashDirection::kLeft:
				{
					CurMontagePtr = LeftMontage;
					Direction = UKismetMathLibrary::MakeRotFromZX(-CharacterPtr->GetGravityDirection(), -Rotation.Quaternion().GetRightVector()).Vector();
				}
				break;
				case EDashDirection::kRight:
				{
					CurMontagePtr = RightMontage;
					Direction = UKismetMathLibrary::MakeRotFromZX(-CharacterPtr->GetGravityDirection(), Rotation.Quaternion().GetRightVector()).Vector();
				}
				break;
				}

				const auto Rate = FMath::IsNearlyZero(Duration) ? CurMontagePtr->CalculateSequenceLength() / Duration : 1.f;
				PlayMontage(CurMontagePtr, Rate);

				Displacement(Direction);

				CharacterPtr->GetEquipmentItemsComponent()->AddTag(UGameplayTagsSubSystem::GetInstance()->DashAbilityTag);
			}
		}
	}

	if (ScopeLockCount <= 0)
	{
		check(0);
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
	}
}

void UPlanetGameplayAbility_Dash::PlayMontage(UAnimMontage* CurMontagePtr, float Rate)
{
	{
		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			CurMontagePtr,
			CharacterPtr->GetMesh()->GetAnimInstance(),
			Rate
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());

		TaskPtr->OnCompleted.BindUObject(this, &ThisClass::DecrementListLock);
		TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::DecrementListLock);

		TaskPtr->ReadyForActivation();

		IncrementListLock();
	}
}

void UPlanetGameplayAbility_Dash::Displacement(const FVector& Direction)
{
	{
		auto TaskPtr = UAbilityTask_MyApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
			this,
			TEXT(""),
			Direction,
			MoveLength / Duration,
			Duration,
			false,
			nullptr,
			ERootMotionFinishVelocityMode::SetVelocity,
			CharacterPtr->GetCharacterMovement()->Velocity,
			0.f,
			false
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());

		TaskPtr->OnFinish.BindUObject(this, &ThisClass::DecrementListLock);

		TaskPtr->ReadyForActivation();

		IncrementListLock();
	}
}
