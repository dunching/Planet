
#include "Skill_Active_Control.h"

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

#include "KismetCollisionHelper.h"
#include "KismetGravityLibrary.h"

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "Tool_PickAxe.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "Planet.h"
#include "CollisionDataStruct.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "InteractiveBaseGAComponent.h"
#include "GameplayTagsSubSystem.h"
#include "CS_RootMotion.h"
#include "BasicFutures_MoveToAttaclArea.h"
#include "PlanetPlayerController.h"
#include "CS_RootMotion_MoveAlongSpline.h"

USkill_Active_Control::USkill_Active_Control() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USkill_Active_Control::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void USkill_Active_Control::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasFocusActor())
	{
		K2_CancelAbility();
	}

	auto bIsHaveTargetInDistance = CheckTargetInDistance(AttackDistance);
	if (bIsHaveTargetInDistance)
	{
		CommitAbility(Handle, ActorInfo, ActivationInfo);

		PerformAction();
	}
	else
	{
		auto DataPtr = dynamic_cast<const FGameplayAbilityTargetData_Control*>(TriggerEventData->TargetData.Get(0));
		if (DataPtr)
		{
			auto MoveToAttaclAreaPtr = new FGameplayAbilityTargetData_MoveToAttaclArea;

			MoveToAttaclAreaPtr->TargetCharacterPtr = Cast<ACharacterBase>(CharacterPtr->GetController<APlanetPlayerController>()->GetFocusActor());
			MoveToAttaclAreaPtr->DataPtr = DataPtr->Clone();
			MoveToAttaclAreaPtr->AttackDistance = AttackDistance;
			MoveToAttaclAreaPtr->CanbeActivedInfoSPtr = DataPtr->CanbeActivedInfoSPtr;

			CharacterPtr->GetInteractiveBaseGAComponent()->MoveToAttackDistance(
				MoveToAttaclAreaPtr
			);

			K2_CancelAbility();
		}
	}
}

bool USkill_Active_Control::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return true;
	}
	return false;
}

void USkill_Active_Control::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	if (SPlineActorPtr)
	{
		SPlineActorPtr->Destroy();
	}
	SPlineActorPtr = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Active_Control::PerformAction()
{
	if (CharacterPtr)
	{
		ExcuteTasks();
		PlayMontage();
	}
}

void USkill_Active_Control::ExcuteTasks()
{
	SPlineActorPtr = GetWorldImp()->SpawnActor<ASPlineActor>(SPlineActorClass, CharacterPtr->GetActorTransform());

	const auto Duration = HumanMontage->CalculateSequenceLength();

	FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(CharacterPtr);
	GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

	auto ICPtr = CharacterPtr->GetInteractiveBaseGAComponent();

	// ÉËº¦
	{
		FGAEventData GAEventData(HasFocusActor(), CharacterPtr);

		GAEventData.SetBaseDamage(Damage);

		GAEventDataPtr->DataAry.Add(GAEventData);
		ICPtr->SendEventImp(GAEventDataPtr);
	}

	// ¿ØÖÆÐ§¹û
	{
		auto GameplayAbilityTargetData_RootMotionPtr = new FGameplayAbilityTargetData_RootMotion_MoveAlongSpline;

		GameplayAbilityTargetData_RootMotionPtr->TriggerCharacterPtr = CharacterPtr;
		GameplayAbilityTargetData_RootMotionPtr->TargetCharacterPtr = HasFocusActor();
		GameplayAbilityTargetData_RootMotionPtr->SPlineActorPtr = SPlineActorPtr;
		GameplayAbilityTargetData_RootMotionPtr->Duration = Duration;

		ICPtr->SendEventImp(GameplayAbilityTargetData_RootMotionPtr);
	}
}

void USkill_Active_Control::PlayMontage()
{
	{
		const float InPlayRate = 1.f;

		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			HumanMontage,
			CharacterPtr->GetMesh()->GetAnimInstance(),
			InPlayRate
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());
		TaskPtr->OnCompleted.BindUObject(this, &ThisClass::K2_CancelAbility);
		TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::K2_CancelAbility);

		TaskPtr->ReadyForActivation();
	}
}

FGameplayAbilityTargetData_ActiveSkill* FGameplayAbilityTargetData_Control::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_Control;

	*ResultPtr = *this;

	return ResultPtr;
}
