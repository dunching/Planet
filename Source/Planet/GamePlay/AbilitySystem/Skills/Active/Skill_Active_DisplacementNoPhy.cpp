
#include "Skill_Active_DisplacementNoPhy.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "BaseFeatureComponent.h"
#include "Tool_PickAxe.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "GameplayTagsLibrary.h"
#include "CollisionDataStruct.h"
#include "CharacterAttributesComponent.h"
#include "AbilityTask_ARM_ConstantForce.h"
#include "AS_Character.h"

USkill_Active_DisplacementNoPhy::USkill_Active_DisplacementNoPhy() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USkill_Active_DisplacementNoPhy::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void USkill_Active_DisplacementNoPhy::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		CommitAbility(Handle, ActorInfo, ActivationInfo);
	}
#endif
}

bool USkill_Active_DisplacementNoPhy::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	if (PP > CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetPP())
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USkill_Active_DisplacementNoPhy::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	CharacterPtr->GetCapsuleComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnComponentBeginOverlap);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool USkill_Active_DisplacementNoPhy::CommitAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
)
{
	// 数值修改
	FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(CharacterPtr);

	GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;
	{
		FGAEventData GAEventData(CharacterPtr, CharacterPtr);

		GAEventData.DataModify.Add(ECharacterPropertyType::PP, -PP);
		GAEventData.DataSource = UGameplayTagsLibrary::DataSource_Character;

		GAEventDataPtr->DataAry.Add(GAEventData);
	}
	auto ICPtr = CharacterPtr->GetBaseFeatureComponent();
	ICPtr->SendEventImp(GAEventDataPtr);

	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void USkill_Active_DisplacementNoPhy::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (CharacterPtr)
	{
		Move();
		PlayMontage();

		CharacterPtr->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
	}
}

void USkill_Active_DisplacementNoPhy::Move()
{
	if (
		(CharacterPtr->GetLocalRole() == ROLE_Authority) ||
		(CharacterPtr->GetLocalRole() == ROLE_AutonomousProxy)
		)
	{
		const FRotator Rotation = CharacterPtr->Controller->GetControlRotation();
		const auto Direction =
			UKismetMathLibrary::MakeRotFromZX(-CharacterPtr->GetGravityDirection(), Rotation.Quaternion().GetForwardVector()).Vector();

		auto TaskPtr = UAbilityTask_ARM_ConstantForce::ApplyRootMotionConstantForce(
			this,
			TEXT(""),
			Direction,
			MoveLength / Duration,
			Duration,
			false,
			true,
			nullptr,
			ERootMotionFinishVelocityMode::ClampVelocity,
			FVector::ZeroVector,
			CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetMoveSpeed(),
			false
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());

		TaskPtr->ReadyForActivation();
	}
}

void USkill_Active_DisplacementNoPhy::InitalDefaultTags()
{
	Super::InitalDefaultTags();

	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::State_NoPhy);
}

void USkill_Active_DisplacementNoPhy::PlayMontage()
{
	if (
		(CharacterPtr->GetLocalRole() == ROLE_Authority) ||
		(CharacterPtr->GetLocalRole() == ROLE_AutonomousProxy)
		)
	{
		const float InPlayRate = HumanMontage->CalculateSequenceLength() / Duration;

		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			HumanMontage,
			InPlayRate
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());
		TaskPtr->OnCompleted.BindUObject(this, &ThisClass::K2_CancelAbility);
		TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::K2_CancelAbility);

		TaskPtr->ReadyForActivation();
	}
}

void USkill_Active_DisplacementNoPhy::OnComponentBeginOverlap(
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
}

void USkill_Active_DisplacementNoPhy::MakeDamage(ACharacterBase* TargetCharacterPtr)
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
