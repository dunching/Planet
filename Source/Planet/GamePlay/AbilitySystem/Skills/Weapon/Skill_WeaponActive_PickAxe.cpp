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
#include "AssetRefMap.h"
#include "AS_Character.h"
#include "Weapon_PickAxe.h"
#include "GE_Common.h"
#include "TeamMatesHelperComponent.h"
#include "HumanCharacter.h"
#include "CharacterAbilitySystemComponent.h"
#include "GameplayTagsLibrary.h"
#include "GroupSharedInfo.h"
#include "ItemProxy_Character.h"

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
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
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

	checkNoEntry();
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
#if UE_EDITOR || UE_CLIENT
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
	)
	{
	}
#endif
	
	if (NotifyName == Skill_WeaponActive_PickAxe::AttackEnd)
	{
		CheckInContinue(-1.f);
	}
	
#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode()  == NM_DedicatedServer)
	{
		if (NotifyName == Skill_WeaponActive_PickAxe::Hit)
		{
			MakeDamage();
		}
	}
#endif
}

void USkill_WeaponActive_PickAxe::OnMontageComplete()
{
#if UE_EDITOR || UE_SERVER
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
	)
	{
		// 
		K2_CancelAbility();
	}
#endif
}

void USkill_WeaponActive_PickAxe::OnMontageOnInterrupted()
{
#if UE_EDITOR || UE_SERVER
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
	)
	{
		// 确认是不在输入，而不是再次输入
		if (!bIsContinue)
		{
		}
	}
#endif
}

void USkill_WeaponActive_PickAxe::MakeDamage()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

	FCollisionQueryParams CapsuleParams;
	CapsuleParams.AddIgnoredActor(CharacterPtr);

	auto GroupMnaggerComponent = Cast<AHumanCharacter>(CharacterPtr)->GetGroupSharedInfo();
	if (GroupMnaggerComponent)
	{
		auto TeamsHelperSPtr = GroupMnaggerComponent->GetTeamMatesHelperComponent();
		if (TeamsHelperSPtr)
		{
			for (auto Iter : TeamsHelperSPtr->MembersSet)
			{
				CapsuleParams.AddIgnoredActor(Iter->GetCharacterActor().Get());
			}
		}
	}

	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(45, 90);

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
		const auto& CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		const int32 BaseDamage = Damage + (CharacterAttributes->GetAD() * AD_Damage_Magnification);

		// FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(
		// 	CharacterPtr);
		//
		// GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

		TSet<ACharacterBase*> TargetCharacterSet;
		for (auto Iter : OutHits)
		{
			auto TargetCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
			if (TargetCharacterPtr)
			{
				TargetCharacterSet.Add(TargetCharacterPtr);
			}
		}

		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(UAssetRefMap::GetInstance()->DamageClass, GetAbilityLevel());
		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);
		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_Damage);
		SpecHandle.Data.Get()->AddDynamicAssetTag(SkillProxyPtr->GetProxyType());

		SpecHandle.Data.Get()->SetSetByCallerMagnitude(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Base,
													   BaseDamage);

		TArray<TWeakObjectPtr<AActor> >Ary;
		for (auto Iter:TargetCharacterSet)
		{
			Ary.Add(Iter);
		}
		FGameplayAbilityTargetDataHandle TargetData;

		auto GameplayAbilityTargetData_ActorArrayPtr = new FGameplayAbilityTargetData_ActorArray;
		GameplayAbilityTargetData_ActorArrayPtr->SetActors(Ary);

		TargetData.Add(GameplayAbilityTargetData_ActorArrayPtr);
		const auto GEHandleAry = MyApplyGameplayEffectSpecToTarget(
			GetCurrentAbilitySpecHandle(),
			GetCurrentActorInfo(),
			GetCurrentActivationInfo(),
			SpecHandle,
			TargetData
		);
		for (auto Iter : TargetCharacterSet)
		{
			// FGAEventData GAEventData(Iter, CharacterPtr);
			//
			// GAEventData.bIsWeaponAttack = true;
			// GAEventData.AttackEffectType = EAttackEffectType::kNormalAttackEffect;
			// GAEventData.SetBaseDamage(BaseDamage);
			//
			// GAEventDataPtr->DataAry.Add(GAEventData);

		}

		// auto ICPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
		// ICPtr->SendEventImp(GAEventDataPtr);
	}
}

void USkill_WeaponActive_PickAxe::PlayMontage()
{
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
	)
	{
		const float Rate = CharacterPtr->GetCharacterAttributesComponent()->GetRate();

		{
			auto AbilityTask_PlayMontage_HumanPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
				this,
				TEXT(""),
				HumanMontage,
				Rate
			);

			AbilityTask_PlayMontage_HumanPtr->Ability = this;
			AbilityTask_PlayMontage_HumanPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());
			AbilityTask_PlayMontage_HumanPtr->OnCompleted.BindUObject(this, &ThisClass::OnMontageComplete);
			AbilityTask_PlayMontage_HumanPtr->OnInterrupted.BindUObject(this, &ThisClass::OnMontageOnInterrupted);

			AbilityTask_PlayMontage_HumanPtr->OnNotifyBegin.BindUObject(this, &ThisClass::OnNotifyBeginReceived);

			AbilityTask_PlayMontage_HumanPtr->ReadyForActivation();
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
			AbilityTask_PlayMontage_PickAxePtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());
			// AbilityTask_PlayMontage_PickAxePtr->OnCompleted.BindUObject(this, &ThisClass::OnMontageComplete);
			// AbilityTask_PlayMontage_PickAxePtr->OnInterrupted.BindUObject(this, &ThisClass::OnMontateComplete);

			AbilityTask_PlayMontage_PickAxePtr->ReadyForActivation();
		}
	}
}
