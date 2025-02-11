
#include "BasicFutures_Affected.h"

#include "CharacterBase.h"
#include "AbilityTask_PlayMontage.h"
#include "GameplayTagsLibrary.h"
#include "AbilityTask_ARM_ConstantForce.h"
#include "CharacterAbilitySystemComponent.h"

UScriptStruct* FGameplayAbilityTargetData_Affected::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_Affected::StaticStruct();
}

bool FGameplayAbilityTargetData_Affected::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << AffectedDirection;
	Ar << RepelDistance;
	Ar << TriggerCharacterPtr;

	return true;
}

UBasicFutures_Affected::UBasicFutures_Affected() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bRetriggerInstancedAbility = true;
}

void UBasicFutures_Affected::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
	}
}

void UBasicFutures_Affected::PreActivate(
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

void UBasicFutures_Affected::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

#ifdef WITH_EDITOR
#endif

	ActiveParamPtr = dynamic_cast<const FGameplayAbilityTargetData_Affected*>(TriggerEventData->TargetData.Get(0));
	if (ActiveParamPtr)
	{
		PerformAction();
	}
	else
	{
		checkNoEntry();
		K2_CancelAbility();
	}
}

bool UBasicFutures_Affected::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	OUT FGameplayTagContainer* OptionalRelevantTags
) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UBasicFutures_Affected::InitalDefaultTags()
{
	// // AbilityTags.AddTag(UGameplayTagsLibrary::Affected);
	//
	// // 在运动时不激活
	// ActivationBlockedTags.AddTag(UGameplayTagsLibrary::RootMotion);
	//
	// // 在“霸体”时不激活
	// ActivationBlockedTags.AddTag(UGameplayTagsLibrary::State_Buff_SuperArmor);
}

void UBasicFutures_Affected::PerformAction()
{
	UAnimMontage* CurMontagePtr = nullptr;

	const FRotator Rotation = CharacterPtr->GetActorRotation();
	FVector Direction = FVector::ZeroVector;

	switch (ActiveParamPtr->AffectedDirection)
	{
	case EAffectedDirection::kForward:
	{
		CurMontagePtr = ForwardMontage;
	}
	break;
	case EAffectedDirection::kBackward:
	{
		CurMontagePtr = BackwardMontage;
	}
	break;
	case EAffectedDirection::kLeft:
	{
		CurMontagePtr = LeftMontage;
	}
	break;
	case EAffectedDirection::kRight:
	{
		CurMontagePtr = RightMontage;
	}
	break;
	}

	const auto Rate = 1.f;
	PlayMontage(CurMontagePtr, Rate);

	if (ActiveParamPtr->RepelDistance > 0)
	{
		Move(CurMontagePtr, Rate);
	}
}

void UBasicFutures_Affected::PlayMontage(UAnimMontage* CurMontagePtr, float Rate)
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

		TaskPtr->OnCompleted.BindUObject(this, &ThisClass::K2_CancelAbility);
		TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::K2_CancelAbility);

		TaskPtr->ReadyForActivation();
	}
}

void UBasicFutures_Affected::Move(UAnimMontage* CurMontagePtr, float Rate)
{
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
		)
	{
		const auto Duration = CurMontagePtr->CalculateSequenceLength();
		const auto Direction =
			ActiveParamPtr->RepelDirection.IsNearlyZero() ?
			(ActiveParamPtr->TriggerCharacterPtr->GetActorLocation() - CharacterPtr->GetActorLocation()).GetSafeNormal() :
			ActiveParamPtr->RepelDirection;

		auto TaskPtr = UAbilityTask_ARM_ConstantForce::ApplyRootMotionConstantForce(
			this,
			TEXT(""),
			Direction,
			ActiveParamPtr->RepelDistance / Duration,
			Duration,
			false,
			false,
			nullptr,
			ERootMotionFinishVelocityMode::ClampVelocity,
			FVector::ZeroVector,
			0.f,// 受击后 速度降为0
			false
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());

		TaskPtr->ReadyForActivation();
	}
}
