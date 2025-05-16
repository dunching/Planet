
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
#include "Weapon_RangeTest.h"
#include "PlanetControllerInterface.h"
#include "GE_Common.h"
#include "Weapon_Bow.h"
#include "CharacterAbilitySystemComponent.h"
#include "GameplayTagsLibrary.h"
#include "KismetGravityLibrary.h"
#include "LogWriter.h"

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
			WeaponActorPtr = Cast<FWeaponActorType>(ActiveParamSPtr->WeaponPtr);
		}
	}
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

void USkill_WeaponActive_Bow::StartTasksLink()
{
	if (WeaponActorPtr && CharacterPtr)
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
		if (OtherCharacterPtr->GetCharacterAbilitySystemComponent()->IsCantBeDamage())
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
	if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
	{
		if (RegisterParamSPtr && RegisterParamSPtr->bIsMultiple)
		{
			EmitProjectile(-30.f);
			EmitProjectile(0.f);
			EmitProjectile(30.f);
		}
		else
		{
			EmitProjectile(0.f);
		}
	}

#if UE_EDITOR || UE_SERVER
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
		)
	{
		if (PerformIfContinue())
		{
		}
		else
		{
			K2_CancelAbility();
		}
	}
#endif
}

void USkill_WeaponActive_Bow::OnMontageOnInterrupted()
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

void USkill_WeaponActive_Bow::EmitProjectile(float OffsetAroundZ)const
{
	auto EmitTransform = WeaponActorPtr->GetEmitTransform();
	const FRotator OffsetRot(0.f,OffsetAroundZ,0.f);
	
	const auto AttackDistance = WeaponActorPtr->GetWeaponProxy()->GetMaxAttackDistance();

	ACharacterBase* HomingTarget = nullptr;
	if (RegisterParamSPtr && RegisterParamSPtr->bIsHomingTowards)
	{
		auto FocusActorPtr = HasFocusActor();
		if (FocusActorPtr)
		{
			HomingTarget = FocusActorPtr;
			auto OrgineRot = (FocusActorPtr->GetActorLocation() - EmitTransform.GetLocation()).Rotation();
			EmitTransform.SetRotation((OrgineRot + OffsetRot).Quaternion());
		}
		else
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
				// 初始角度
				const auto OriginRot = UKismetMathLibrary::MakeRotFromZX(
					-UKismetGravityLibrary::GetGravity(),
					CharacterPtr->GetActorForwardVector()
				);
				const auto Dir = (OriginRot + OffsetRot).Vector();
				const auto CharacterLocation = CharacterPtr->GetActorLocation();
				
				float Dot = 0.f;

				for (const auto& Iter : OutHits)
				{
					auto OtherCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
					if (CharacterPtr->IsGroupmate(OtherCharacterPtr))
					{
						continue;
					}

					const auto TargetDir = (OtherCharacterPtr->GetActorLocation() - CharacterLocation).GetSafeNormal();
					const auto NewDot = FVector::DotProduct(TargetDir, Dir);
					if (NewDot  > Dot)
					{
						Dot = NewDot;
						HomingTarget = OtherCharacterPtr;
					}
				}
				
				EmitTransform.SetRotation((OriginRot + OffsetRot).Quaternion());
			}
		}
	}
	else
	{
		auto OrgineRot = UKismetMathLibrary::MakeRotFromZX(
			-UKismetGravityLibrary::GetGravity(),
			CharacterPtr->GetActorForwardVector()
		);
		EmitTransform.SetRotation((OrgineRot +OffsetRot).Quaternion());
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
	const auto& CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
	const int32 BaseDamage = Elemental_Damage;

	FGameplayEffectSpecHandle SpecHandle =
		MakeOutgoingGameplayEffectSpec(UAssetRefMap::GetInstance()->OnceGEClass, GetAbilityLevel());
	SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);
	SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_Damage);
	SpecHandle.Data.Get()->AddDynamicAssetTag(SkillProxyPtr->GetProxyType());

	SpecHandle.Data.Get()->SetSetByCallerMagnitude(UGameplayTagsLibrary::GEData_ModifyItem_Damage_Metal,
												   BaseDamage);

	TArray<TWeakObjectPtr<AActor> >Ary;
	Ary.Add(TargetCharacterPtr);
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
}

void USkill_WeaponActive_Bow::PlayMontage()
{
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
		)
	{ 
		const auto GAPerformSpeed = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetPerformSpeed();
		const float Rate = static_cast<float>(GAPerformSpeed) / 100;

		{
			auto AbilityTask_PlayMontage_HumanPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
				this,
				TEXT(""),
				HumanMontage,
				Rate
			);

			AbilityTask_PlayMontage_HumanPtr->Ability = this;
			AbilityTask_PlayMontage_HumanPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());
			AbilityTask_PlayMontage_HumanPtr->OnCompleted.BindUObject(this, &ThisClass::OnMontateComplete);
			AbilityTask_PlayMontage_HumanPtr->OnInterrupted.BindUObject(this, &ThisClass::K2_CancelAbility);

			AbilityTask_PlayMontage_HumanPtr->OnNotifyBegin.BindUObject(this, &ThisClass::OnNotifyBeginReceived);

			AbilityTask_PlayMontage_HumanPtr->ReadyForActivation();
		}
		{
			auto AbilityTask_PlayMontage_PickAxePtr = UAbilityTask_PlayMontage::CreatePlayMontageAndWaitProxy(
				this,
				TEXT(""),
				BowMontage,
				WeaponActorPtr->GetMesh()->GetAnimInstance(),
				Rate
			);

			AbilityTask_PlayMontage_PickAxePtr->Ability = this;
			AbilityTask_PlayMontage_PickAxePtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());

			AbilityTask_PlayMontage_PickAxePtr->ReadyForActivation();
		}
	}
}
