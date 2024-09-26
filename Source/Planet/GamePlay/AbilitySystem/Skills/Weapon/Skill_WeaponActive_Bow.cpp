
#include "Skill_WeaponActive_Bow.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"
#include "GameFramework/Controller.h"

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "UnitProxyProcessComponent.h"
#include "ToolFuture_Base.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "Planet.h"
#include "CollisionDataStruct.h"
#include "CharacterAttributesComponent.h"
#include "AbilityTask_TimerHelper.h"
#include "Weapon_PickAxe.h"
#include "Weapon_RangeTest.h"
#include "PlanetControllerInterface.h"
#include "GroupMnaggerComponent.h"
#include "Weapon_Bow.h"
#include "BaseFeatureComponent.h"

namespace Skill_WeaponActive_RangeTest
{
	const FName Hit = TEXT("Hit");

	const FName AttackEnd = TEXT("AttackEnd");
}

USkill_WeaponActive_Bow::USkill_WeaponActive_Bow() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USkill_WeaponActive_Bow::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void USkill_WeaponActive_Bow::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData /*= nullptr */)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		if (ActiveParamPtr)
		{
			WeaponPtr = Cast<FWeaponActorType>(ActiveParamPtr->WeaponPtr);
		}
	}
}

void USkill_WeaponActive_Bow::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (WeaponPtr)
	{
		return;
	}

	check(0);
	K2_EndAbility();
}

void USkill_WeaponActive_Bow::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void USkill_WeaponActive_Bow::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	StartTasksLink();
}

void USkill_WeaponActive_Bow::StartTasksLink()
{
	if (WeaponPtr && CharacterPtr)
	{
		PlayMontage();
	}
}

void USkill_WeaponActive_Bow::OnProjectileBounce(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (OtherActor && OtherActor->IsA(ACharacterBase::StaticClass()))
	{
		auto OtherCharacterPtr = Cast<ACharacterBase>(OtherActor);
		if (CharacterPtr->IsGroupmate(OtherCharacterPtr))
		{
			return;
		}

		MakeDamage(OtherCharacterPtr);
	}

	OverlappedComponent->GetOwner()->Destroy();
}

void USkill_WeaponActive_Bow::OnNotifyBeginReceived(FName NotifyName)
{
	if (NotifyName == Skill_WeaponActive_RangeTest::AttackEnd)
	{
		EmitProjectile();

		CheckInContinue();
	}
}

void USkill_WeaponActive_Bow::OnMontateComplete()
{
	K2_CancelAbility();
}

void USkill_WeaponActive_Bow::EmitProjectile()
{
	auto ProjectilePtr = GetWorld()->SpawnActor<ASkill_WeaponActive_Bow_Projectile>(
		Skill_WeaponActive_RangeTest_ProjectileClass,
		CharacterPtr->GetActorLocation(),
		CharacterPtr->GetActorRotation()
	);
	if (ProjectilePtr)
	{
		ProjectilePtr->CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnProjectileBounce);
	}
}

void USkill_WeaponActive_Bow::MakeDamage(ACharacterBase* TargetCharacterPtr)
{
	FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(CharacterPtr);

	GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

	if (TargetCharacterPtr)
	{
		FGAEventData GAEventData(TargetCharacterPtr, CharacterPtr);

		GAEventData.SetBaseDamage(Damage);

		GAEventDataPtr->DataAry.Add(GAEventData);
	}

	auto ICPtr = CharacterPtr->GetBaseFeatureComponent();
	ICPtr->SendEventImp(GAEventDataPtr);
}

void USkill_WeaponActive_Bow::PlayMontage()
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
		AbilityTask_PlayMontage_HumanPtr->OnCompleted.BindUObject(this, &ThisClass::OnMontateComplete);
		AbilityTask_PlayMontage_HumanPtr->OnInterrupted.BindUObject(this, &ThisClass::OnMontateComplete);

		AbilityTask_PlayMontage_HumanPtr->OnNotifyBegin.BindUObject(this, &ThisClass::OnNotifyBeginReceived);

		AbilityTask_PlayMontage_HumanPtr->ReadyForActivation();
	}
}
