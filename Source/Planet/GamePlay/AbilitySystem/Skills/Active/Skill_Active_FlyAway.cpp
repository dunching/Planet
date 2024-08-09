
#include "Skill_Active_FlyAway.h"

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

USkill_Active_FlyAway::USkill_Active_FlyAway() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USkill_Active_FlyAway::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void USkill_Active_FlyAway::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	PerformAction();
}

bool USkill_Active_FlyAway::CanActivateAbility(
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

void USkill_Active_FlyAway::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Active_FlyAway::PerformAction()
{
	if (CharacterPtr)
	{
		ExcuteTasks();
		PlayMontage();
	}
}

void USkill_Active_FlyAway::ExcuteTasks()
{
	if (CharacterPtr)
	{
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(CharacterPtr);

		DrawDebugLine(
			GetWorld(), 
			CharacterPtr->GetActorLocation(),
			CharacterPtr->GetActorLocation() + (CharacterPtr->GetActorRotation().Vector() * 100),
			FColor::Red, false, 3
		);

		DrawDebugLine(
			GetWorld(),
			CharacterPtr->GetActorLocation(),
			CharacterPtr->GetActorLocation() + (CharacterPtr->GetControlRotation().Vector() * 100),
			FColor::Yellow, false, 3
		);

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

		auto ICPtr = CharacterPtr->GetInteractiveBaseGAComponent();

		TSet <ACharacterBase*>TargetSet;
		for (const auto & Iter : Result)
		{
			auto TargetCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
			if (TargetCharacterPtr)
			{
				TargetSet.Add(TargetCharacterPtr);
			}
		}

		// ÉËº¦
		for (const auto& Iter : TargetSet)
		{
			FGAEventData GAEventData(Iter, CharacterPtr);

			GAEventData.SetBaseDamage(Damage);

			GAEventDataPtr->DataAry.Add(GAEventData);
		}
		ICPtr->SendEventImp(GAEventDataPtr);

		// ¿ØÖÆÐ§¹û
		for (const auto& Iter : TargetSet)
		{
			auto GAEventData_Periodic_StateTagModifyPtr = new FGameplayAbilityTargetData_Periodic_RootMotion(
				UGameplayTagsSubSystem::GetInstance()->FlyAway,
				FlyAwayTime
			);

			GAEventData_Periodic_StateTagModifyPtr->Height = Height;

			GAEventData_Periodic_StateTagModifyPtr->TriggerCharacterPtr = CharacterPtr;
			GAEventData_Periodic_StateTagModifyPtr->TargetCharacterPtr = Iter;

			ICPtr->SendEventImp(GAEventData_Periodic_StateTagModifyPtr);
		}
	}
}

void USkill_Active_FlyAway::PlayMontage()
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
