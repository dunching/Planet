
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
#include "Kismet/KismetMathLibrary.h"

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
#include "AS_Character.h"
#include "Weapon_PickAxe.h"
#include "Weapon_RangeTest.h"
#include "PlanetControllerInterface.h"
#include "TeamMatesHelperComponent.h"
#include "Weapon_Bow.h"
#include "BaseFeatureComponent.h"
#include "KismetGravityLibrary.h"

namespace Skill_WeaponActive_Bow
{
	const FName Hit = TEXT("Hit");

	const FName AttackEnd = TEXT("AttackEnd");
}

UScriptStruct* FGameplayAbilityTargetData_Bow_RegisterParam::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_Bow_RegisterParam::StaticStruct();
}

bool FGameplayAbilityTargetData_Bow_RegisterParam::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << bIsHomingTowards;
	Ar << bIsMultiple;

	return true;
}

FGameplayAbilityTargetData_Bow_RegisterParam* FGameplayAbilityTargetData_Bow_RegisterParam::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_Bow_RegisterParam;

	*ResultPtr = *this;

	return ResultPtr;
}

USkill_WeaponActive_Bow::USkill_WeaponActive_Bow() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USkill_WeaponActive_Bow::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void USkill_WeaponActive_Bow::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, 
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		if (ActiveParamSPtr)
		{
			WeaponPtr = Cast<FWeaponActorType>(ActiveParamSPtr->WeaponPtr);
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

	checkNoEntry();
	K2_EndAbility();
}

void USkill_WeaponActive_Bow::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void USkill_WeaponActive_Bow::UpdateRegisterParam(const FGameplayEventData& GameplayEventData)
{
	Super::UpdateRegisterParam(GameplayEventData);

	if (GameplayEventData.TargetData.IsValid(0))
	{
		RegisterParamSPtr = MakeSPtr_GameplayAbilityTargetData<FRegisterParamType>(GameplayEventData.TargetData.Get(0));
	}
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

void USkill_WeaponActive_Bow::CheckInContinue()
{
	if (bIsContinue)
	{
		PerformAction(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), &CurrentEventData);
	}
	else
	{
		if (
			(CharacterPtr->GetLocalRole() == ROLE_Authority) ||
			(CharacterPtr->GetLocalRole() == ROLE_AutonomousProxy)
			)
		{
			K2_CancelAbility();
		}
	}
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
	if (NotifyName == Skill_WeaponActive_Bow::AttackEnd)
	{
	}
}

void USkill_WeaponActive_Bow::OnMontateComplete()
{
	if (CharacterPtr->GetLocalRole() == ROLE_Authority)
	{
		EmitProjectile();
	}

	if (
		(CharacterPtr->GetLocalRole() == ROLE_Authority) ||
		(CharacterPtr->GetLocalRole() == ROLE_AutonomousProxy)
		)
	{
		CheckInContinue();
	}
}

void USkill_WeaponActive_Bow::EmitProjectile()const
{
	auto EmitTransform = WeaponPtr->GetEmitTransform();

	const auto AttackDistance = WeaponPtr->GetWeaponProxy()->GetMaxAttackDistance();

	ACharacterBase* HomingTarget = nullptr;
	if (RegisterParamSPtr && RegisterParamSPtr->bIsHomingTowards)
	{
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

		FCollisionShape CollisionShape = FCollisionShape::MakeSphere(SweepWidth);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(CharacterPtr);

		TArray<struct FHitResult> OutHits;
		if (CharacterPtr->GetWorld()->SweepMultiByObjectType(
			OutHits,
			EmitTransform.GetLocation(),
			EmitTransform.GetLocation() + (CharacterPtr->GetActorForwardVector() * AttackDistance),
			FQuat::Identity,
			ObjectQueryParams,
			CollisionShape,
			Params
		))
		{
			for (const auto& Iter : OutHits)
			{
				auto OtherCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
				if (CharacterPtr->IsGroupmate(OtherCharacterPtr))
				{
					continue;
				}
				else if (FVector::Distance(OtherCharacterPtr->GetActorLocation(), CharacterPtr->GetActorLocation()) < SweepWidth)
				{
					continue;
				}
				EmitTransform.SetRotation(UKismetMathLibrary::MakeRotFromZX(
					-UKismetGravityLibrary::GetGravity(),
					CharacterPtr->GetActorForwardVector()
				).Quaternion());

				HomingTarget = OtherCharacterPtr;

				break;
			}
		}
	}
	else
	{
		auto FocusActorPtr = HasFocusActor();
		if (FocusActorPtr)
		{
			EmitTransform.SetRotation((FocusActorPtr->GetActorLocation() - EmitTransform.GetLocation()).ToOrientationQuat());
		}
		else
		{
			EmitTransform.SetRotation(UKismetMathLibrary::MakeRotFromZX(
				-UKismetGravityLibrary::GetGravity(),
				CharacterPtr->GetActorForwardVector()
			).Quaternion());
		}
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.CustomPreSpawnInitalization = [AttackDistance](AActor* ActorPtr)
		{
			Cast<ASkill_WeaponActive_Bow_Projectile>(ActorPtr)->MaxMoveRange = AttackDistance;
		};

	auto ProjectilePtr = GetWorld()->SpawnActor<ASkill_WeaponActive_Bow_Projectile>(
		Skill_WeaponActive_RangeTest_ProjectileClass,
		EmitTransform,
		SpawnParameters
	);

	if (ProjectilePtr)
	{
		if (HomingTarget)
		{
			ProjectilePtr->SetHomingTarget(HomingTarget);
		}
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
	const auto GAPerformSpeed = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetPerformSpeed();
	const float Rate = static_cast<float>(GAPerformSpeed) / 100;

	if (
		(CharacterPtr->GetLocalRole() == ROLE_Authority) ||
		(CharacterPtr->GetLocalRole() == ROLE_AutonomousProxy)
		)
	{ 
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
			AbilityTask_PlayMontage_HumanPtr->OnInterrupted.BindUObject(this, &ThisClass::K2_CancelAbility);

			AbilityTask_PlayMontage_HumanPtr->ReadyForActivation();
		}
		{
			auto AbilityTask_PlayMontage_PickAxePtr = UAbilityTask_PlayMontage::CreatePlayMontageAndWaitProxy(
				this,
				TEXT(""),
				BowMontage,
				WeaponPtr->GetMesh()->GetAnimInstance(),
				Rate
			);

			AbilityTask_PlayMontage_PickAxePtr->Ability = this;
			AbilityTask_PlayMontage_PickAxePtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());

			AbilityTask_PlayMontage_PickAxePtr->ReadyForActivation();
		}
	}
}
