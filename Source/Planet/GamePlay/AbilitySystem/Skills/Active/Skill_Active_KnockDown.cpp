
#include "Skill_Active_KnockDown.h"

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
#include "GameFramework/CharacterMovementComponent.h"

#include "KismetCollisionHelper.h"
#include "KismetGravityLibrary.h"

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "Tool_PickAxe.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "Planet.h"
#include "CollisionDataStruct.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "BaseFeatureComponent.h"
#include "GameplayTagsSubSystem.h"
#include "CS_RootMotion.h"
#include "CS_RootMotion_FlyAway.h"
#include "CS_RootMotion_KnockDown.h"

USkill_Active_KnockDown::USkill_Active_KnockDown() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USkill_Active_KnockDown::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void USkill_Active_KnockDown::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommitAbility(Handle, ActorInfo, ActivationInfo);
}

bool USkill_Active_KnockDown::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	if (CharacterPtr->GetCharacterMovement()->IsFlying() || CharacterPtr->GetCharacterMovement()->IsFalling())
	{
		return false;
	}
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USkill_Active_KnockDown::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Active_KnockDown::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (CharacterPtr)
	{
		ExcuteTasks();
		PlayMontage();
	}
}

void USkill_Active_KnockDown::ExcuteTasks()
{
	if (CharacterPtr)
	{
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(CharacterPtr);

		const auto Dir = UKismetMathLibrary::MakeRotFromZX(
			UKismetGravityLibrary::GetGravity(CharacterPtr->GetActorLocation()), CharacterPtr->GetControlRotation().Vector()
		).Vector();

		auto Result = UKismetCollisionHelper::OverlapMultiSectorByObjectType(
			GetWorld(),
			CharacterPtr->GetActorLocation(),
			CharacterPtr->GetActorLocation() + (Dir * Radius),
			Angle,
			9,
			ObjectQueryParams,
			Params
		);

		FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(CharacterPtr);
		GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

		auto ICPtr = CharacterPtr->GetBaseFeatureComponent();

		TSet <ACharacterBase*>TargetSet;
		for (const auto & Iter : Result)
		{
			auto TargetCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
			if (TargetCharacterPtr && !CharacterPtr->IsGroupmate(TargetCharacterPtr))
			{
				TargetSet.Add(TargetCharacterPtr);
			}
		}

		// 伤害
		for (const auto& Iter : TargetSet)
		{
			FGAEventData GAEventData(Iter, CharacterPtr);

			GAEventData.SetBaseDamage(Damage);

			GAEventDataPtr->DataAry.Add(GAEventData);
		}
		ICPtr->SendEventImp(GAEventDataPtr);

		// 控制效果
		for (const auto& Iter : TargetSet)
		{
			auto GameplayAbilityTargetData_RootMotionPtr = new FGameplayAbilityTargetData_RootMotion_KnockDown;

			GameplayAbilityTargetData_RootMotionPtr->TriggerCharacterPtr = CharacterPtr;
			GameplayAbilityTargetData_RootMotionPtr->TargetCharacterPtr = Iter;

			ICPtr->SendEventImp(GameplayAbilityTargetData_RootMotionPtr);
		}
	}
}

void USkill_Active_KnockDown::PlayMontage()
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
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());
		TaskPtr->OnCompleted.BindUObject(this, &ThisClass::K2_CancelAbility);
		TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::K2_CancelAbility);

		TaskPtr->ReadyForActivation();
	}
}
