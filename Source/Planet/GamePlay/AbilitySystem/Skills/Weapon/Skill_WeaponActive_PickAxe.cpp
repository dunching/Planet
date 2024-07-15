
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

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
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
#include "InteractiveBaseGAComponent.h"

namespace Skill_WeaponActive_PickAxe
{
	const FName Hit = TEXT("Hit");

	const FName AttackEnd = TEXT("AttackEnd");
}

USkill_WeaponActive_PickAxe::USkill_WeaponActive_PickAxe() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USkill_WeaponActive_PickAxe::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (CharacterPtr)
	{
		auto & CharacterAttributesRef= CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		CharacterAttributesRef.BaseAttackPower.AddCurrentValue(AD, PropertuModify_GUID);
		CharacterAttributesRef.Penetration.AddCurrentValue(AD_Penetration, PropertuModify_GUID);
		CharacterAttributesRef.PercentPenetration.AddCurrentValue(AD_PercentPenetration, PropertuModify_GUID);
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
		auto& CharacterAttributesRef = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		CharacterAttributesRef.BaseAttackPower.RemoveCurrentValue(PropertuModify_GUID);
		CharacterAttributesRef.Penetration.RemoveCurrentValue(PropertuModify_GUID);
		CharacterAttributesRef.PercentPenetration.RemoveCurrentValue(PropertuModify_GUID);
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

void USkill_WeaponActive_PickAxe::PerformAction()
{
	Super::PerformAction();

	StartTasksLink();
}

bool USkill_WeaponActive_PickAxe::IsEnd() const
{
	return Super::IsEnd();
}

void USkill_WeaponActive_PickAxe::StartTasksLink()
{
	if (EquipmentAxePtr && CharacterPtr)
	{
		PlayMontage();
	}
}

void USkill_WeaponActive_PickAxe::OnNotifyBeginReceived(FName NotifyName)
{
	if (NotifyName == Skill_WeaponActive_PickAxe::AttackEnd)
	{
		MakeDamage();

		SkillState = EType::kAttackingEnd;
		if (!bIsRequstCancel)
		{
			DecrementToZeroListLock();
		}
	}
}

void USkill_WeaponActive_PickAxe::MakeDamage()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

	FCollisionShape  CollisionShape = FCollisionShape::MakeCapsule(45, 90);

	FCollisionQueryParams CapsuleParams;
	CapsuleParams.AddIgnoredActor(CharacterPtr);

	auto GroupMnaggerComponent = Cast<AHumanCharacter>(CharacterPtr)->GetGroupMnaggerComponent();
	if (GroupMnaggerComponent)
	{
		auto TeamsHelperSPtr = GroupMnaggerComponent->GetTeamHelper();
		if (TeamsHelperSPtr)
		{
			for (auto Iter : TeamsHelperSPtr->MembersMap)
			{
				CapsuleParams.AddIgnoredActor(Iter.Value);
			}
		}
	}
	TMap<ACharacterBase*, TMap<ECharacterPropertyType, FBaseProperty>> ModifyPropertyMap;
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
		for (auto Iter : OutHits)
		{
			auto TargetCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
			if (TargetCharacterPtr)
			{
				decltype(ModifyPropertyMap)::ValueType Value;

				Value.Add(ECharacterPropertyType::kHP, FBaseProperty(Damage));

				ModifyPropertyMap.Add(TargetCharacterPtr, Value);
			}
		}

	}
	
	auto ICPtr = CharacterPtr->GetInteractiveBaseGAComponent();
	ICPtr->SendEvent2Other(ModifyPropertyMap, true);
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
