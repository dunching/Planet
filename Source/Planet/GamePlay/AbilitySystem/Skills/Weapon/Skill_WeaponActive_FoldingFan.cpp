
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

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
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
#include "Weapon_FoldingFan.h"
#include "BaseFeatureComponent.h"
#include "KismetGravityLibrary.h"
#include "AbilityTask_FlyAway.h"
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

	check(0);
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
		check(0);
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

void USkill_WeaponActive_FoldingFan::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, CurrentFanNum, COND_AutonomousOnly);
}

void USkill_WeaponActive_FoldingFan::UpdateParam(const FGameplayEventData& GameplayEventData)
{
	Super::UpdateParam(GameplayEventData);

	if (!GameplayEventData.TargetData.IsValid(0))
	{
		check(0);
	}
	RegisterParamSPtr = MakeSPtr_GameplayAbilityTargetData<FRegisterParamType>(GameplayEventData.TargetData.Get(0));
	if (!RegisterParamSPtr)
	{
		check(0);
	}

#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetLocalRole() == ROLE_Authority)
	{
		CurrentFanNum += RegisterParamSPtr->IncreaseNum;
	}
#endif
}

void USkill_WeaponActive_FoldingFan::CheckInContinue()
{
	if (
		bIsContinue && 
		!RootMotionPtr &&
		CanActivateAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()) 
		)
	{
		PerformAction(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), &CurrentEventData);
	}
	else
	{
	}
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
		if (OtherCharacterPtr == CharacterPtr)
		{
			auto ActivedWeaponPtr = Cast<FWeaponActorType>(OverlappedComponent->GetOwner());
			if (ActivedWeaponPtr && ActivedWeaponPtr->bIsReachFarestPoint)
			{
				CurrentFanNum++;
				if (WeaponPtr)
				{
					WeaponPtr->SetActorHiddenInGame(false);
				}

				ActivedWeaponPtr->Destroy();

#if UE_EDITOR || UE_SERVER
				if (CharacterPtr->GetLocalRole() == ROLE_Authority)
				{
					OnCurrentFanNumChanged();
				}
#endif
				return;
			}
		}
		
		if (CharacterPtr->IsGroupmate(OtherCharacterPtr))
		{
			return;
		}

		MakeDamage(OtherCharacterPtr);
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
	if (CharacterPtr->GetLocalRole() == ROLE_Authority)
	{
		CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());
	}
#endif

	EmitProjectile();
}

void USkill_WeaponActive_FoldingFan::OnMotionComplete()
{
	RootMotionPtr = nullptr;

	if (bIsContinue)
	{
		CheckInContinue();
	}
	else
	{
#if UE_EDITOR || UE_SERVER
		if (CharacterPtr->GetLocalRole() == ROLE_Authority)
		{
			K2_CancelAbility();
		}
#endif
	}
}

void USkill_WeaponActive_FoldingFan::EmitProjectile()
{
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetLocalRole() == ROLE_Authority)
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
			ActivedWeaponPtr->SetWeaponUnit(WeaponPtr->WeaponProxyPtr->GetID());
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

void USkill_WeaponActive_FoldingFan::MakeDamage(ACharacterBase* TargetCharacterPtr)
{
	auto GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(CharacterPtr);

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

void USkill_WeaponActive_FoldingFan::PlayMontage()
{
	const auto GAPerformSpeed = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().GAPerformSpeed.GetCurrentValue();
	const float Rate = static_cast<float>(GAPerformSpeed) / 100;

	if (
		(CharacterPtr->GetLocalRole() == ROLE_Authority) ||
		(CharacterPtr->GetLocalRole() == ROLE_AutonomousProxy)
		)
	{
		AbilityTask_PlayMontage_HumanPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			HumanMontage,
			CharacterPtr->GetMesh()->GetAnimInstance(),
			Rate
		);

		AbilityTask_PlayMontage_HumanPtr->Ability = this;
		AbilityTask_PlayMontage_HumanPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());
		AbilityTask_PlayMontage_HumanPtr->OnCompleted.BindUObject(this, &ThisClass::OnMontateComplete);
		AbilityTask_PlayMontage_HumanPtr->OnInterrupted.BindUObject(this, &ThisClass::K2_CancelAbility);

		AbilityTask_PlayMontage_HumanPtr->ReadyForActivation();
	}
}

void USkill_WeaponActive_FoldingFan::RootMotion()
{
	if (CharacterPtr->GetLocalRole() > ROLE_SimulatedProxy)
	{
		const auto Lenth = HumanMontage->CalculateSequenceLength();

		RootMotionPtr = UAbilityTask_FlyAway::NewTask(
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
