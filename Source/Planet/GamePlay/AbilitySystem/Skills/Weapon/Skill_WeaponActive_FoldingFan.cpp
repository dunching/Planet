
#include "Skill_WeaponActive_FoldingFan.h"

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
#include "Net/UnrealNetwork.h"


#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "ToolFuture_Base.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "PlanetModule.h"
#include "CollisionDataStruct.h"
#include "CharacterAttributesComponent.h"
#include "AbilityTask_TimerHelper.h"
#include "Weapon_PickAxe.h"
#include "Weapon_RangeTest.h"
#include "PlanetControllerInterface.h"
#include "TeamMatesHelperComponentBase.h"
#include "Weapon_FoldingFan.h"
#include "CharacterAbilitySystemComponent.h"
#include "KismetGravityLibrary.h"
#include "AbilityTask_ApplyRootMotion_FlyAway.h"
#include "AS_Character.h"
#include "ItemProxy_Weapon.h"
#include "LogWriter.h"

namespace Skill_WeaponActive_FoldingFan
{
	const FName Hit = TEXT("Hit");

	const FName AttackEnd = TEXT("AttackEnd");
}

UScriptStruct* FGameplayAbilityTargetData_FoldingFan_RegisterParam::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_FoldingFan_RegisterParam::StaticStruct();
}

bool FGameplayAbilityTargetData_FoldingFan_RegisterParam::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << IncreaseNum;

	return true;
}

FGameplayAbilityTargetData_FoldingFan_RegisterParam* FGameplayAbilityTargetData_FoldingFan_RegisterParam::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_FoldingFan_RegisterParam;

	*ResultPtr = *this;

	return ResultPtr;
}

USkill_WeaponActive_FoldingFan::USkill_WeaponActive_FoldingFan() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USkill_WeaponActive_FoldingFan::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void USkill_WeaponActive_FoldingFan::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, 
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	AbilityTask_PlayMontage_HumanPtr = nullptr;

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		if (ActiveParamSPtr)
		{
			WeaponPtr = Cast<FWeaponActorType>(ActiveParamSPtr->WeaponPtr);
		}
	}
}

void USkill_WeaponActive_FoldingFan::ActivateAbility(
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

bool USkill_WeaponActive_FoldingFan::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	if (CurrentFanNum < 1)
	{
		return false;
	}
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

bool USkill_WeaponActive_FoldingFan::CommitAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
)
{
	CurrentFanNum--;

	if (CurrentFanNum < 0)
	{
		checkNoEntry();
		CurrentFanNum = 0;
	}

	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void USkill_WeaponActive_FoldingFan::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void USkill_WeaponActive_FoldingFan::UpdateRegisterParam(const FGameplayEventData& GameplayEventData)
{
	Super::UpdateRegisterParam(GameplayEventData);

	if (!GameplayEventData.TargetData.IsValid(0))
	{
		checkNoEntry();
	}
	RegisterParamSPtr = MakeSPtr_GameplayAbilityTargetData<FRegisterParamType>(GameplayEventData.TargetData.Get(0));
	if (!RegisterParamSPtr)
	{
		checkNoEntry();
	}

#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
	{
		CurrentFanNum += RegisterParamSPtr->IncreaseNum;
	}
#endif
}

bool USkill_WeaponActive_FoldingFan::GetNum(int32& Num) const
{
	Num = CurrentFanNum;

	return true;
}

void USkill_WeaponActive_FoldingFan::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	StartTasksLink();
}

void USkill_WeaponActive_FoldingFan::StartTasksLink()
{
	if (WeaponPtr && CharacterPtr)
	{
		PlayMontage();
		RootMotion();
	}
}

void USkill_WeaponActive_FoldingFan::OnProjectileBounce(
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
		if (OtherCharacterPtr)
		{
			auto ActivedWeaponPtr = Cast<FWeaponActorType>(OverlappedComponent->GetOwner());
			if (!ActivedWeaponPtr)
			{
				return;
			}

			if (OtherCharacterPtr == CharacterPtr)
			{
				if (ActivedWeaponPtr && ActivedWeaponPtr->bIsReachFarestPoint)
				{
					CurrentFanNum++;
					if (WeaponPtr)
					{
						WeaponPtr->SetActorHiddenInGame(false);
					}

					ActivedWeaponPtr->Destroy();

#if UE_EDITOR || UE_SERVER
					if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
					{
						OnCurrentFanNumChanged();
					}
#endif
					return;
				}
			}
			else 
			{
				if (CharacterPtr->IsGroupmate(OtherCharacterPtr))
				{
					return;
				}

				MakeDamage(OtherCharacterPtr, ActivedWeaponPtr);
			}
		}
		else
		{
		}
	}
}

void USkill_WeaponActive_FoldingFan::OnCurrentFanNumChanged()
{
//	WeaponPtr->SetActorHiddenInGame(CurrentFanNum <= 0);
}

void USkill_WeaponActive_FoldingFan::OnNotifyBeginReceived(FName NotifyName)
{
	if (NotifyName == Skill_WeaponActive_FoldingFan::AttackEnd)
	{
	}
}

void USkill_WeaponActive_FoldingFan::OnMontateComplete()
{
	AbilityTask_PlayMontage_HumanPtr = nullptr;

#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
	{
		CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());
	}
#endif

	EmitProjectile();
}

void USkill_WeaponActive_FoldingFan::OnMotionComplete()
{
	RootMotionPtr = nullptr;

	if (GetIsContinue())
	{
		PerformIfContinue();
	}
	else
	{
#if UE_EDITOR || UE_SERVER
		if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
		{
			K2_CancelAbility();
		}
#endif
	}
}

void USkill_WeaponActive_FoldingFan::EmitProjectile()
{
#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
	{
		FActorSpawnParameters SpawnParameters;

		SpawnParameters.Owner = CharacterPtr;

		auto ActivedWeaponPtr = GWorld->SpawnActor<FWeaponActorType>(
			WeaponPtr->GetClass(),
			WeaponPtr->GetTransform(),
			SpawnParameters
		);
		if (ActivedWeaponPtr)
		{
			ActivedWeaponPtr->SetWeaponProxy(WeaponPtr->GetWeaponProxy()->GetID());
			ActivedWeaponPtr->GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnProjectileBounce);
			ActivedWeaponPtr->BeginAttack();
		}

		if (CurrentFanNum <= 0)
		{
			WeaponPtr->SetActorHiddenInGame(true);
		}
	}
#endif
}

void USkill_WeaponActive_FoldingFan::MakeDamage(ACharacterBase* TargetCharacterPtr, FWeaponActorType* WeaponProjectilePtr)
{
	if (TargetCharacterPtr && WeaponProjectilePtr)
	{
	}
}

void USkill_WeaponActive_FoldingFan::PlayMontage()
{
	const auto GAPerformSpeed = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetPerformSpeed();
	const float Rate = static_cast<float>(GAPerformSpeed) / 100;

	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
		)
	{
		AbilityTask_PlayMontage_HumanPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			HumanMontage,
			Rate
		);

		AbilityTask_PlayMontage_HumanPtr->Ability = this;
		AbilityTask_PlayMontage_HumanPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());
		AbilityTask_PlayMontage_HumanPtr->OnCompleted.BindUObject(this, &ThisClass::OnMontateComplete);
		AbilityTask_PlayMontage_HumanPtr->OnInterrupted.BindUObject(this, &ThisClass::K2_CancelAbility);

		AbilityTask_PlayMontage_HumanPtr->ReadyForActivation();
	}
}

void USkill_WeaponActive_FoldingFan::RootMotion()
{
	if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() > ROLE_SimulatedProxy)
	{
		const auto Lenth = HumanMontage->CalculateSequenceLength();

		RootMotionPtr = UAbilityTask_ApplyRootMotion_FlyAway::NewTask(
			this,
			TEXT(""),
			ERootMotionAccumulateMode::Additive,
			Lenth,
			Height,
			ResingSpeed,
			FallingSpeed
		);

		RootMotionPtr->OnFinished.BindUObject(this, &ThisClass::OnMotionComplete);
		RootMotionPtr->ReadyForActivation();
	}
}
