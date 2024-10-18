
#include "Skill_Active_Fear.h"

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
#include "CS_PeriodicStateModify_Fear.h"

bool USkill_Active_Fear::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	if (
		CharacterPtr->GetCharacterMovement()->IsFlying() ||
		CharacterPtr->GetCharacterMovement()->IsFalling()
		)
	{
		return false;
	}
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USkill_Active_Fear::PerformAction(
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
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		CommitAbility(Handle, ActorInfo, ActivationInfo);
	}
#endif
}

void USkill_Active_Fear::ExcuteTasks()
{
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
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

			TSet<ACharacterBase*>TargetSet;
			for (const auto& Iter : Result)
			{
				auto TargetCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
				if (TargetCharacterPtr && !CharacterPtr->IsGroupmate(TargetCharacterPtr))
				{
					TargetSet.Add(TargetCharacterPtr);
				}
			}

			// 伤害
			auto ICPtr = CharacterPtr->GetBaseFeatureComponent();

			auto GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(CharacterPtr);
			GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

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
				auto GameplayAbilityTargetData_RootMotionPtr = new FGameplayAbilityTargetData_StateModify_Fear(Duration);

				GameplayAbilityTargetData_RootMotionPtr->TriggerCharacterPtr = CharacterPtr;
				GameplayAbilityTargetData_RootMotionPtr->TargetCharacterPtr = Iter;

				ICPtr->SendEventImp(GameplayAbilityTargetData_RootMotionPtr);
			}
		}
	}
#endif
}

void USkill_Active_Fear::PlayMontage()
{
	if (
		(CharacterPtr->GetLocalRole() == ROLE_Authority) ||
		(CharacterPtr->GetLocalRole() == ROLE_AutonomousProxy)
		)
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
