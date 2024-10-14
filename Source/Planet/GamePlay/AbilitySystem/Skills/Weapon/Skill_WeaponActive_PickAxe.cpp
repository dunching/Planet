
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
#include "ProxyProcessComponent.h"
#include "ToolFuture_Base.h"
#include "AbilityTask_PlayMontage.h"
#include "AbilityTask_PlayAnimAndWaitOverride.h"
#include "ToolFuture_PickAxe.h"
#include "Planet.h"
#include "CollisionDataStruct.h"
#include "CharacterAttributesComponent.h"
#include "AbilityTask_TimerHelper.h"
#include "Weapon_PickAxe.h"
#include "PlanetControllerInterface.h"
#include "GroupMnaggerComponent.h"
#include "HumanCharacter.h"
#include "BaseFeatureComponent.h"

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
	}
}

void USkill_WeaponActive_PickAxe::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec
)
{
	// Ins Or Spec

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
		if (ActiveParamSPtr)
		{
			EquipmentAxePtr = Cast<AWeapon_PickAxe>(ActiveParamSPtr->WeaponPtr);
		}
	}
}

void USkill_WeaponActive_PickAxe::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (EquipmentAxePtr)
	{
		return;
	}

	check(0);
	K2_EndAbility();
}

void USkill_WeaponActive_PickAxe::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	MontageNum = 0;

	StartTasksLink();
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
#if UE_EDITOR || UE_SERVER
		if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
		{
			MakeDamage();
		}
#endif

		CheckInContinue();
	}
}

void USkill_WeaponActive_PickAxe::OnMontateComplete()
{
	MontageNum--;
	if ((MontageNum <= 0) && (CharacterPtr->GetLocalRole() == ROLE_AutonomousProxy))
	{
		K2_CancelAbility();
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
			for (auto Iter : TeamsHelperSPtr->MembersSet)
			{
				CapsuleParams.AddIgnoredActor(Iter->ProxyCharacterPtr.Get());
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
		const auto & CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		const auto BaseDamage = Damage + CharacterAttributes.AD.GetCurrentValue();

		FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(CharacterPtr);

		GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

		for (auto Iter : OutHits)
		{
			auto TargetCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
			if (TargetCharacterPtr)
			{
				FGAEventData GAEventData(TargetCharacterPtr, CharacterPtr);

				GAEventData.bIsWeaponAttack = true;
				GAEventData.bIsMakeAttackEffect = true;
				GAEventData.SetBaseDamage(BaseDamage);

				GAEventDataPtr->DataAry.Add(GAEventData);
			}
		}

		auto ICPtr = CharacterPtr->GetBaseFeatureComponent();
		ICPtr->SendEventImp(GAEventDataPtr);
	}
}

void USkill_WeaponActive_PickAxe::PlayMontage()
{
	if (
		(CharacterPtr->GetLocalRole() == ROLE_Authority) ||
		(CharacterPtr->GetLocalRole() == ROLE_AutonomousProxy)
		)
	{
		const auto GAPerformSpeed = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().GAPerformSpeed.GetCurrentValue();
		const float Rate = static_cast<float>(GAPerformSpeed) / 100;

		{
			auto AbilityTask_PlayMontage_HumanPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
				this,
				TEXT(""),
				HumanMontage,
				Rate
			);

			AbilityTask_PlayMontage_HumanPtr->Ability = this;
			AbilityTask_PlayMontage_HumanPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());
			AbilityTask_PlayMontage_HumanPtr->OnCompleted.BindUObject(this, &ThisClass::OnMontateComplete);
			AbilityTask_PlayMontage_HumanPtr->OnInterrupted.BindUObject(this, &ThisClass::OnMontateComplete);

			AbilityTask_PlayMontage_HumanPtr->OnNotifyBegin.BindUObject(this, &ThisClass::OnNotifyBeginReceived);

			AbilityTask_PlayMontage_HumanPtr->ReadyForActivation();

			MontageNum++;
		}
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
			AbilityTask_PlayMontage_PickAxePtr->OnCompleted.BindUObject(this, &ThisClass::OnMontateComplete);
			AbilityTask_PlayMontage_PickAxePtr->OnInterrupted.BindUObject(this, &ThisClass::OnMontateComplete);

			AbilityTask_PlayMontage_PickAxePtr->ReadyForActivation();

			MontageNum++;
		}
	}
}
