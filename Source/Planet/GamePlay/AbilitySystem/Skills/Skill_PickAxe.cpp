
#include "Skill_PickAxe.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"

#include "GAEvent.h"
#include "CharacterBase.h"
#include "EquipmentElementComponent.h"
#include "ToolFuture_Base.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "Planet.h"
#include "CollisionDataStruct.h"
#include "CharacterAttributesComponent.h"
#include "AbilityTask_TimerHelper.h"
#include "Weapon_PickAxe.h"

const FName Hit = TEXT("Hit");

const FName AttachEnd = TEXT("AttachEnd");

USkill_PickAxe1::USkill_PickAxe1() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bRetriggerInstancedAbility = true;
}

void USkill_PickAxe1::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD.AddCurrentValue(15);
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD_Penetration.AddCurrentValue(10);
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD_PercentPenetration.AddCurrentValue(5);
	}
}

void USkill_PickAxe1::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData /*= nullptr */)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	RepeatType = ERepeatType::kInfinte;

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		auto GameplayAbilityTargetData_DashPtr = dynamic_cast<const FGameplayAbilityTargetData_Skill_PickAxe*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetData_DashPtr)
		{
			EquipmentAxePtr = GameplayAbilityTargetData_DashPtr->WeaponPtr;
			if (GameplayAbilityTargetData_DashPtr->WeaponPtr)
			{
				return;
			}
		}
	}

	check(0);
	K2_EndAbility();
}

bool USkill_PickAxe1::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */) const
{
	switch (RepeatType)
	{
	case ERepeatType::kStop:
	{
		if (!bIsAttackEnd)
		{
			return false;
		}
	}
	break;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USkill_PickAxe1::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	// Ins Or Spec
	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD.AddCurrentValue(-15);
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD_Penetration.AddCurrentValue(-10);
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD_PercentPenetration.AddCurrentValue(-5);
	}

	Super::OnAvatarSet(ActorInfo, Spec);
}

void USkill_PickAxe1::ExcuteStepsLink()
{
	StartTasksLink();
}

void USkill_PickAxe1::StartTasksLink()
{
	if (EquipmentAxePtr && CharacterPtr)
	{
		bIsAttackEnd = false;
		PlayMontage();
	}
}

void USkill_PickAxe1::OnNotifyBeginReceived(FName NotifyName)
{
	if (NotifyName == Hit)
	{
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(PawnECC);

		FCollisionShape  CollisionShape = FCollisionShape::MakeCapsule(45, 90);

		FCollisionQueryParams CapsuleParams;
		CapsuleParams.AddIgnoredActor(CharacterPtr);

		TArray<struct FHitResult> OutHits;
		if (GetWorldImp()->SweepMultiByObjectType(
			OutHits,
			CharacterPtr->GetActorLocation(),
			CharacterPtr->GetActorLocation() + (CharacterPtr->GetActorForwardVector() * Distance),
			FQuat::Identity,
			ObjectQueryParams,
			CollisionShape,
			CapsuleParams
		))
		{
			FGameplayAbilityTargetData_GAEvent* GAEventData = new FGameplayAbilityTargetData_GAEvent;

			FGameplayEventData Payload;
			Payload.TargetData.Add(GAEventData);

			GAEventData->TargetActorAry.Empty();
			GAEventData->TriggerCharacterPtr = CharacterPtr;
			GAEventData->ADDamage = Damage;

			for (auto Iter : OutHits)
			{
				auto TargetCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
				if (TargetCharacterPtr)
				{
					GAEventData->TargetActorAry.Add(TargetCharacterPtr);
				}
			}

			SendEvent(Payload);
		}
	}
	else if (NotifyName == AttachEnd)
	{
		bIsAttackEnd = true;
		if (RepeatType != USkill_Base::ERepeatType::kStop)
		{
			DecrementToZeroListLock();
		}
	}
}

void USkill_PickAxe1::PlayMontage()
{
	const auto GAPerformSpeed = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().GAPerformSpeed.GetCurrentValue();
	const float Rate = static_cast<float>(GAPerformSpeed) / 100;

	{
		auto AbilityTask_PlayMontage_PickAxePtr = UAbilityTask_PlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			PickAxeMontage,
			EquipmentAxePtr->GetMesh()->GetAnimInstance(),
			Rate
		);
		
		AbilityTask_PlayMontage_PickAxePtr->Ability = this;
		AbilityTask_PlayMontage_PickAxePtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());
		AbilityTask_PlayMontage_PickAxePtr->OnCompleted.BindUObject(this, &ThisClass::DecrementListLockOverride);
		AbilityTask_PlayMontage_PickAxePtr->OnInterrupted.BindUObject(this, &ThisClass::DecrementListLockOverride);

		AbilityTask_PlayMontage_PickAxePtr->ReadyForActivation();

		IncrementListLock();
	}
	{
		auto AbilityTask_PlayMontage_HumanPtr = UAbilityTask_PlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			HumanMontage,
			CharacterPtr->GetMesh()->GetAnimInstance(),
			Rate
		);

		AbilityTask_PlayMontage_HumanPtr->Ability = this;
		AbilityTask_PlayMontage_HumanPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());
		AbilityTask_PlayMontage_HumanPtr->OnCompleted.BindUObject(this, &ThisClass::DecrementListLockOverride);
		AbilityTask_PlayMontage_HumanPtr->OnInterrupted.BindUObject(this, &ThisClass::DecrementListLockOverride);

		AbilityTask_PlayMontage_HumanPtr->OnNotifyBegin.BindUObject(this, &ThisClass::OnNotifyBeginReceived);

		AbilityTask_PlayMontage_HumanPtr->ReadyForActivation();

		IncrementListLock();
	}
}
