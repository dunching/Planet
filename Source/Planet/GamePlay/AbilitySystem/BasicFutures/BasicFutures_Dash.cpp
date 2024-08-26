// Copyright 2020 Dan Kestranek.

#include "BasicFutures_Dash.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Kismet/KismetMathLibrary.h"

#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "AbilityTask_PlayMontage.h"
#include "AbilityTask_MyApplyRootMotionConstantForce.h"
#include "AssetRefMap.h"
#include "GameplayTagsSubSystem.h"
#include "InteractiveSkillComponent.h"
#include "InteractiveToolComponent.h"
#include "InteractiveBaseGAComponent.h"
#include "Planet_Tools.h"

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
		AbilityTags.AddTag(UGameplayTagsSubSystem::GetInstance()->Dash);

		FAbilityTriggerData AbilityTriggerData;

		AbilityTriggerData.TriggerTag = UGameplayTagsSubSystem::GetInstance()->Dash;
		AbilityTriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;

		AbilityTriggers.Add(AbilityTriggerData);
	}
}

void UBasicFutures_Dash::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
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

	CommitAbility(Handle, ActorInfo, ActivationInfo);

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
		FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(CharacterPtr);

		GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

		FGAEventData GAEventData(CharacterPtr, CharacterPtr);

		GAEventData.PP = Consume;

		GAEventDataPtr->DataAry.Add(GAEventData);

		auto ICPtr = CharacterPtr->GetInteractiveBaseGAComponent();
		ICPtr->SendEventImp(GAEventDataPtr);
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
	if (CharacterPtr)
	{
		auto CharacterAttributesSPtr = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		if (CharacterAttributesSPtr->PP.GetCurrentValue() >= Consume)
		{
			return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
		}
	}

	return false;
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
			}
		}
	}
}

void UBasicFutures_Dash::PlayMontage(UAnimMontage* CurMontagePtr, float Rate)
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

		TaskPtr->OnCompleted.BindUObject(this, &ThisClass::DecrementListLockOverride);
		TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::DecrementListLockOverride);

		TaskPtr->ReadyForActivation();

		IncrementListLock();
	}
}

void UBasicFutures_Dash::Displacement(const FVector& Direction)
{
	{
		auto TaskPtr = UAbilityTask_MyApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
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
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());

		// TaskPtr->OnFinish.BindUObject(this, &ThisClass::DecrementListLockOverride);

		// 如果遇到障碍 提前结束
		TaskPtr->OnFinish.BindUObject(this, &ThisClass::DecrementToZeroListLock);

		TaskPtr->ReadyForActivation();

		IncrementListLock();
	}
}
