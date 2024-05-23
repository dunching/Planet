
#include "Skill_WeaponActive_RangeTest.h"

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
#include "Weapon_RangeTest.h"
#include "HumanControllerInterface.h"
#include "GroupMnaggerComponent.h"
#include "HumanCharacter.h"

namespace Skill_PickAxe
{
	const FName Hit = TEXT("Hit");

	const FName AttackEnd = TEXT("AttackEnd");
}

USkill_WeaponActive_RangeTest::USkill_WeaponActive_RangeTest() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bRetriggerInstancedAbility = true;
}

void USkill_WeaponActive_RangeTest::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void USkill_WeaponActive_RangeTest::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData /*= nullptr */)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	RepeatType = ERepeatType::kInfinte;

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		auto GameplayAbilityTargetData_DashPtr = dynamic_cast<const FGameplayAbilityTargetData_Skill_WeaponActive_RangeTest*>(TriggerEventData->TargetData.Get(0));
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

bool USkill_WeaponActive_RangeTest::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */) const
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

void USkill_WeaponActive_RangeTest::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void USkill_WeaponActive_RangeTest::ExcuteStepsLink()
{
	StartTasksLink();
}

void USkill_WeaponActive_RangeTest::StartTasksLink()
{
	if (EquipmentAxePtr && CharacterPtr)
	{
		bIsAttackEnd = false;
		PlayMontage();
	}
}

void USkill_WeaponActive_RangeTest::OnNotifyBeginReceived(FName NotifyName)
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

void USkill_WeaponActive_RangeTest::MakeDamage()
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

void USkill_WeaponActive_RangeTest::PlayMontage()
{
	const auto GAPerformSpeed = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().GAPerformSpeed.GetCurrentValue();
	const float Rate = static_cast<float>(GAPerformSpeed) / 100;

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
