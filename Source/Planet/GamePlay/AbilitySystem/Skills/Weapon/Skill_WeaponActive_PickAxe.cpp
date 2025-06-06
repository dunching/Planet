#include "Skill_WeaponActive_PickAxe.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Controller.h"

#include "CharacterBase.h"
#include "ToolFuture_Base.h"
#include "AbilityTask_PlayMontage.h"
#include "AbilityTask_PlayAnimAndWaitOverride.h"
#include "ToolFuture_PickAxe.h"
#include "CollisionDataStruct.h"
#include "CharacterAttributesComponent.h"
#include "Weapon_PickAxe.h"
#include "TeamMatesHelperComponentBase.h"
#include "HumanCharacter.h"
#include "CharacterAbilitySystemComponent.h"
#include "SceneProxyTable.h"
#include "GroupManagger.h"
#include "ItemProxy_Character.h"
#include "ItemProxy_Skills.h"
#include "LogWriter.h"
#include "PlanetGameplayAbilityTargetTypes.h"
#include "TeamMatesHelperComponent.h"
#include "Tools.h"

struct FSkill_WeaponActive_PickAxe : public TStructVariable<FSkill_WeaponActive_PickAxe>
{
	const FName Hit = TEXT("Hit");

	const FName AttackEnd = TEXT("AttackEnd");
};

UScriptStruct* FGameplayAbilityTargetData_Axe_RegisterParam::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_Axe_RegisterParam::StaticStruct();
}

bool FGameplayAbilityTargetData_Axe_RegisterParam::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	return true;
}

FGameplayAbilityTargetData_Axe_RegisterParam* FGameplayAbilityTargetData_Axe_RegisterParam::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_Axe_RegisterParam;

	*ResultPtr = *this;

	return ResultPtr;
}

USkill_WeaponActive_PickAxe::USkill_WeaponActive_PickAxe() :
                                                           Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USkill_WeaponActive_PickAxe::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
	)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (CharacterPtr)
	{
		if (SkillProxyPtr)
		{
			ItemProxy_DescriptionPtr = Cast<FItemProxy_DescriptionType>(
			                                                            DynamicCastSharedPtr<FWeaponSkillProxy>(
				                                                             SkillProxyPtr
				                                                            )->GetTableRowProxy_WeaponSkillExtendInfo()
			                                                           );
		}
	}
}

bool USkill_WeaponActive_PickAxe::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
	) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USkill_WeaponActive_PickAxe::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
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
	const FGameplayEventData* TriggerEventData /*= nullptr */
	)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		if (ActiveParamSPtr)
		{
			WeaponActorPtr = Cast<AWeapon_PickAxe>(ActiveParamSPtr->WeaponPtr);
		}
	}
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
	if (WeaponActorPtr)
	{
		StartTasksLink();
	}
	else
	{
		PRINTINVOKEWITHSTR(FString(TEXT("No Weapon")));
		K2_EndAbility();
	}
}

void USkill_WeaponActive_PickAxe::StartTasksLink()
{
	if (WeaponActorPtr && CharacterPtr)
	{
		PlayMontage();
	}
}

void USkill_WeaponActive_PickAxe::OnNotifyBeginReceived(
	FName NotifyName
	)
{
#if UE_EDITOR || UE_SERVER
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
	)
	{
		if (NotifyName == FSkill_WeaponActive_PickAxe::Get().AttackEnd)
		{
			EnableMovement(true);
			PerformIfContinue();
		}
	}
#endif

#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
	{
		if (NotifyName == FSkill_WeaponActive_PickAxe::Get().Hit)
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
		K2_CancelAbility();
	}
#endif
}

void USkill_WeaponActive_PickAxe::MakeDamage()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

	FCollisionQueryParams CapsuleParams;
	CapsuleParams.AddIgnoredActor(CharacterPtr);

	auto GroupMnaggerComponent = Cast<AHumanCharacter>(CharacterPtr)->GetGroupManagger();
	if (GroupMnaggerComponent)
	{
		auto TeamsHelperSPtr = GroupMnaggerComponent->GetTeamMatesHelperComponent();
		if (TeamsHelperSPtr)
		{
			for (auto Iter : TeamsHelperSPtr->GetMembersSet())
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
	                                          CharacterPtr->GetActorLocation() + (
		                                          CharacterPtr->GetActorForwardVector() * Distance),
	                                          FQuat::Identity,
	                                          ObjectQueryParams,
	                                          CollisionShape,
	                                          CapsuleParams
	                                         ))
	{
		TSet<ACharacterBase*> TargetCharacterSet;
		for (auto Iter : OutHits)
		{
			auto TargetCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
			if (TargetCharacterPtr)
			{
				if (CharacterPtr->IsGroupmate(TargetCharacterPtr))
				{
					continue;
				}
				if (TargetCharacterPtr->GetCharacterAbilitySystemComponent()->IsCantBeDamage())
				{
					continue;
				}
				TargetCharacterSet.Add(TargetCharacterPtr);
			}
		}

		FGameplayEffectSpecHandle SpecHandle = MakeDamageToTargetSpecHandle(
		                                                          ItemProxy_DescriptionPtr->ElementalType,
		                                                          ItemProxy_DescriptionPtr->Elemental_Damage,
		                                                          ItemProxy_DescriptionPtr->Elemental_Damage_Magnification
		                                                         );

		TArray<TWeakObjectPtr<AActor>> Ary;
		for (auto Iter : TargetCharacterSet)
		{
			Ary.Add(Iter);
		}
		FGameplayAbilityTargetDataHandle TargetData;

		auto GameplayAbilityTargetData_ActorArrayPtr = new FGameplayAbilityTargetData_MyActorArray;
		GameplayAbilityTargetData_ActorArrayPtr->SetActors(Ary);
		GameplayAbilityTargetData_ActorArrayPtr->HitResult.ImpactNormal = CharacterPtr->GetActorForwardVector();

		TargetData.Add(GameplayAbilityTargetData_ActorArrayPtr);
		const auto GEHandleAry = MyApplyGameplayEffectSpecToTarget(
		                                                           GetCurrentAbilitySpecHandle(),
		                                                           GetCurrentActorInfo(),
		                                                           GetCurrentActivationInfo(),
		                                                           SpecHandle,
		                                                           TargetData
		                                                          );
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
			AbilityTask_PlayMontage_HumanPtr->SetAbilitySystemComponent(
																		CharacterPtr->
																		GetCharacterAbilitySystemComponent()
																	   );
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
				 WeaponActorPtr->GetMesh()->GetAnimInstance(),
				 Rate
				);

			AbilityTask_PlayMontage_PickAxePtr->Ability = this;
			AbilityTask_PlayMontage_PickAxePtr->SetAbilitySystemComponent(
																		  CharacterPtr->
																		  GetCharacterAbilitySystemComponent()
																		 );
			// AbilityTask_PlayMontage_PickAxePtr->OnCompleted.BindUObject(this, &ThisClass::OnMontageComplete);
			// AbilityTask_PlayMontage_PickAxePtr->OnInterrupted.BindUObject(this, &ThisClass::OnMontateComplete);

			AbilityTask_PlayMontage_PickAxePtr->ReadyForActivation();
		}
	}
}