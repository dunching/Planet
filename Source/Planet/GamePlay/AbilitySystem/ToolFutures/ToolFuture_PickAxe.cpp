
#include "ToolFuture_PickAxe.h"

#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"

#include "CollisionDataStruct.h"
#include "CharacterBase.h"
#include "ArticleBase.h"
#include "AssetRefrencePath.h"
#include "HumanCharacter.h"
#include "Animation/AnimInstanceBase.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolUnitBase.h"
#include "Tool_PickAxe.h"

UToolFuture_PickAxe::UToolFuture_PickAxe() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UToolFuture_PickAxe::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bIsKeepAction = true;
	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());

		auto GameplayAbilityTargetData_DashPtr = dynamic_cast<const FGameplayAbilityTargetData_ToolFuture_PickAxe*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetData_DashPtr)
		{
			EquipmentAxePtr = GameplayAbilityTargetData_DashPtr->EquipmentAxePtr;
		}

		PerformAction();
	}
	else
	{
		check(0);
		K2_EndAbility();
	}
}

bool UToolFuture_PickAxe::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return true;
	}
	return false;
}

void UToolFuture_PickAxe::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	bIsKeepAction = false;
}

void UToolFuture_PickAxe::PerformAction()
{
	if (EquipmentAxePtr && CharacterPtr && bIsKeepAction)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ThisClass::K2_CancelAbility));
		{
			const auto Len = PickAxeMontage->CalculateSequenceLength();
			auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
				this,
				TEXT(""),
				PickAxeMontage,
				EquipmentAxePtr->GetMesh()->GetAnimInstance(),
				Len
			);

			TaskPtr->Ability = this;
			TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());
			TaskPtr->OnCompleted.BindUObject(this, &ThisClass::DecrementListLock);
			TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::DecrementListLock);

			TaskPtr->ReadyForActivation();

			IncrementListLock();
		}
		{
			const auto Len = HumanMontage->CalculateSequenceLength();
			auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
				this,
				TEXT(""),
				HumanMontage,
				CharacterPtr->GetMesh()->GetAnimInstance(),
				Len
			);

			TaskPtr->Ability = this;
			TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());
			TaskPtr->OnCompleted.BindUObject(this, &ThisClass::DecrementListLock);
			TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::DecrementListLock);

			TaskPtr->ReadyForActivation();

			IncrementListLock();
		}
	}
}