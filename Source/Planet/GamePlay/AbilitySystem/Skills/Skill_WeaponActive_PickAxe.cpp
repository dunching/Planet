
#include "Skill_WeaponActive_PickAxe.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"
#include "GameFramework/Controller.h"

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
#include "HumanControllerInterface.h"
#include "GroupMnaggerComponent.h"
#include "HumanCharacter.h"

namespace Skill_PickAxe
{
	const FName Hit = TEXT("Hit");

	const FName AttackEnd = TEXT("AttackEnd");
}

USkill_WeaponActive_PickAxe::USkill_WeaponActive_PickAxe() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bRetriggerInstancedAbility = true;
}

void USkill_WeaponActive_PickAxe::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD.AddCurrentValue(AD);
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD_Penetration.AddCurrentValue(AD_Penetration);
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD_PercentPenetration.AddCurrentValue(AD_PercentPenetration);
	}
}

void USkill_WeaponActive_PickAxe::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec
)
{
	// Ins Or Spec
	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD.AddCurrentValue(-AD);
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD_Penetration.AddCurrentValue(-AD_Penetration);
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD_PercentPenetration.AddCurrentValue(-AD_PercentPenetration);
	}

	Super::OnRemoveAbility(ActorInfo, Spec);
}

void USkill_WeaponActive_PickAxe::PreActivate(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */)
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

bool USkill_WeaponActive_PickAxe::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
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

void USkill_WeaponActive_PickAxe::ExcuteStepsLink()
{
	StartTasksLink();
}

void USkill_WeaponActive_PickAxe::StartTasksLink()
{
	if (EquipmentAxePtr && CharacterPtr)
	{
		bIsAttackEnd = false;
		PlayMontage();
	}
}

void USkill_WeaponActive_PickAxe::OnNotifyBeginReceived(FName NotifyName)
{
	if (NotifyName == Skill_PickAxe::AttackEnd)
	{
		MakeDamage();

		bIsAttackEnd = true;
		if (RepeatType != USkill_Base::ERepeatType::kStop)
		{
			DecrementToZeroListLock();
		}
	}
}

void USkill_WeaponActive_PickAxe::MakeDamage()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(PawnECC);

	FCollisionShape  CollisionShape = FCollisionShape::MakeCapsule(45, 90);

	FCollisionQueryParams CapsuleParams;
	CapsuleParams.AddIgnoredActor(CharacterPtr);

	auto GroupMnaggerComponent = Cast<AHumanCharacter>(CharacterPtr)->GetGroupMnaggerComponent();
	if (GroupMnaggerComponent)
	{
		auto TeamsHelperSPtr = GroupMnaggerComponent->GetTeamsHelper();
		if (TeamsHelperSPtr)
		{
			for (auto Iter : TeamsHelperSPtr->MembersMap)
			{
				CapsuleParams.AddIgnoredActor(Iter.Value);
			}
		}
	}

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
		GAEventData->Data.ADDamage = Damage;

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

void USkill_WeaponActive_PickAxe::PlayMontage()
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
		auto AbilityTask_PlayMontage_HumanPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
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