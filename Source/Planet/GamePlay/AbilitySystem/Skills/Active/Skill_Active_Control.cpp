
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


#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "Tool_PickAxe.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "Planet.h"
#include "CollisionDataStruct.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "CharacterAbilitySystemComponent.h"
#include "GameplayTagsLibrary.h"
#include "BasicFutures_MoveToAttaclArea.h"
#include "PlanetPlayerController.h"

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

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		ActiveParamPtr = dynamic_cast<const ActiveParamType*>(TriggerEventData->TargetData.Get(0));
		if (ActiveParamPtr)
		{
		}
		else
		{
			return;
		}
	}
}

void USkill_Active_Control::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
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

void USkill_Active_Control::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (!HasFocusActor())
	{
		K2_CancelAbility();
	}

	auto bIsHaveTargetInDistance = CheckTargetInDistance(AttackDistance);
	if (bIsHaveTargetInDistance)
	{
		CommitAbility(Handle, ActorInfo, ActivationInfo);

		if (CharacterPtr)
		{
			ExcuteTasks();
			PlayMontage();
		}
	}
	else
	{
		auto DataPtr = dynamic_cast<const FGameplayAbilityTargetData_Control*>(TriggerEventData->TargetData.Get(0));
		if (DataPtr)
		{
			auto MoveToAttaclAreaPtr = new FGameplayAbilityTargetData_MoveToAttaclArea;

			MoveToAttaclAreaPtr->TargetCharacterPtr = Cast<ACharacterBase>(CharacterPtr->GetController<APlanetPlayerController>()->GetFocusActor());
			MoveToAttaclAreaPtr->AttackDistance = AttackDistance;

			CharacterPtr->GetCharacterAbilitySystemComponent()->MoveToAttackDistance(
				MoveToAttaclAreaPtr
			);
		}
	}
}

void USkill_Active_Control::ExcuteTasks()
{
	SPlineActorPtr = GetWorldImp()->SpawnActor<ASPlineActor>(SPlineActorClass, CharacterPtr->GetActorTransform());

	const auto Duration = HumanMontage->CalculateSequenceLength();

}

void USkill_Active_Control::PlayMontage()
{
	{
		const float InPlayRate = 1.f;

		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			HumanMontage,
			InPlayRate
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());
		TaskPtr->OnCompleted.BindUObject(this, &ThisClass::K2_CancelAbility);
		TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::K2_CancelAbility);

		TaskPtr->ReadyForActivation();
	}
}

UScriptStruct* FGameplayAbilityTargetData_Control::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_Control::StaticStruct();
}

bool FGameplayAbilityTargetData_Control::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << TargetCharacterPtr;

	return true;
}

FGameplayAbilityTargetData_ActiveSkill_ActiveParam* FGameplayAbilityTargetData_Control::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_Control;

	*ResultPtr = *this;

	return ResultPtr;
}
