
#include "Skill_WeaponActive_HandProtection.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "Planet.h"
#include "CollisionDataStruct.h"
#include "CharacterAttributesComponent.h"
#include "AbilityTask_TimerHelper.h"
#include "AS_Character.h"
#include "Weapon_HandProtection.h"
#include "UIManagerSubSystem.h"
#include "ProgressTips.h"
#include "Skill_Active_Base.h"
#include "CharacterAbilitySystemComponent.h"
#include "GameplayTagsLibrary.h"
#include "TemplateHelper.h"

namespace Skill_WeaponHandProtection
{
	const FName Hit = TEXT("Hit");

	const FName AttackEnd = TEXT("AttackEnd");
}

USkill_WeaponActive_HandProtection::USkill_WeaponActive_HandProtection() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USkill_WeaponActive_HandProtection::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (CharacterPtr)
	{
	}
}

void USkill_WeaponActive_HandProtection::PreActivate(
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
		if (ActiveParamSPtr)
		{
			WeaponPtr = Cast<AWeapon_HandProtection>(ActiveParamSPtr->WeaponPtr);
		}
	}
}

void USkill_WeaponActive_HandProtection::ActivateAbility(
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

bool USkill_WeaponActive_HandProtection::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USkill_WeaponActive_HandProtection::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_WeaponActive_HandProtection::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (CurrentIndex >= 3)
	{
		CurrentIndex = 0;
	}

	switch (CurrentIndex)
	{
	case 0:
	{
		FirstStep();

		CurrentIndex++;
	}
	break;
	case 1:
	{
		SecondStep();

		CurrentIndex++;
	};
	break;
	case 2:
	default:
	{
		ThirdStep();

		CurrentIndex++;
	}
	break;
	}
}

void USkill_WeaponActive_HandProtection::ResetPreviousStageActions()
{
	Super::ResetPreviousStageActions();
}

void USkill_WeaponActive_HandProtection::ExcuteStopStep()
{
	auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);

	TaskPtr->SetDuration(InputRangeInSecond);

	TaskPtr->OnFinished.BindLambda([this](UAbilityTask_TimerHelper* TaskPtr) {
		CurrentIndex = 0;

		return true;
		});

	TaskPtr->ReadyForActivation();
}

void USkill_WeaponActive_HandProtection::FirstStep()
{
	if (WeaponPtr && CharacterPtr)
	{
		PlayMontage();
	}
}

void USkill_WeaponActive_HandProtection::SecondStep()
{
	if (WeaponPtr && CharacterPtr)
	{
		PlayMontage();
	}
}

void USkill_WeaponActive_HandProtection::ThirdStep()
{
	if (WeaponPtr && CharacterPtr)
	{
		PlayMontage();
	}
}

void USkill_WeaponActive_HandProtection::OnNotifyBeginReceived(FName NotifyName)
{
	if (NotifyName == Skill_WeaponHandProtection::AttackEnd)
	{
#if UE_EDITOR || UE_SERVER
		if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
		{
			MakeDamage();
		}
#endif

		CheckInContinue();
	}
}

void USkill_WeaponActive_HandProtection::OnMontateComplete()
{
}

void USkill_WeaponActive_HandProtection::MakeDamage()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

	FCollisionShape  CollisionShape = FCollisionShape::MakeCapsule(45, 90);

	FCollisionQueryParams CapsuleParams;
	CapsuleParams.AddIgnoredActor(CharacterPtr);

	TArray<struct FHitResult> OutHits;
	if (GetWorldImp()->SweepMultiByObjectType(
		OutHits,
		CharacterPtr->GetActorLocation(),
		CharacterPtr->GetActorLocation() + (CharacterPtr->GetActorForwardVector() * Distance),
		FQuat::Identity,
		ObjectQueryParams,
		CollisionShape,
		CapsuleParams
	))
	{
		FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(CharacterPtr);

		GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

		for (auto Iter : OutHits)
		{
			auto TargetCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
			if (TargetCharacterPtr)
			{
				FGAEventData GAEventData(TargetCharacterPtr, CharacterPtr);

				GAEventData.SetBaseDamage(Damage);

				GAEventDataPtr->DataAry.Add(GAEventData);
			}
		}

		auto SkillsAry = CharacterPtr->GetProxyProcessComponent()->GetAllSocket();
		auto GASPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
		for (const auto& Iter : SkillsAry)
		{
// 			if (Iter.Value->SkillProxyPtr)
// 			{
// 				if (Iter.Value->SkillProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Active))
// 				{
// 					auto ActiveSkillProxyPtr = DynamicCastSharedPtr<FActiveSkillProxy>(Iter.Value->SkillProxyPtr);
// 					if (!ActiveSkillProxyPtr)
// 					{
// 						continue;
// 					}
// 
// 					ActiveSkillProxyPtr->AddCooldownConsumeTime(1.f);
// 				}
// 			}
		}

		auto ICPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
		ICPtr->SendEventImp(GAEventDataPtr);
	}
}

void USkill_WeaponActive_HandProtection::PlayMontage()
{
	const auto GAPerformSpeed = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->GetPerformSpeed();
	const float Rate = static_cast<float>(GAPerformSpeed) / 100;

	{
		// 
	}
	{
		UAnimMontage* HumanMontage = nullptr;

		switch (CurrentIndex)
		{
		case 0:
		{
			HumanMontage = HumanStep1Montage;
		}
		break;
		case 1:
		{
			HumanMontage = HumanStep2Montage;
		}
		break;
		case 2:
		{
			HumanMontage = HumanStep3Montage;
		}
		break;
		}

		auto AbilityTask_PlayMontage_HumanPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			HumanMontage,
			Rate
		);

		AbilityTask_PlayMontage_HumanPtr->Ability = this;
		AbilityTask_PlayMontage_HumanPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());
		AbilityTask_PlayMontage_HumanPtr->OnCompleted.BindUObject(this, &ThisClass::OnMontateComplete);
		AbilityTask_PlayMontage_HumanPtr->OnInterrupted.BindUObject(this, &ThisClass::OnMontateComplete);

		AbilityTask_PlayMontage_HumanPtr->OnNotifyBegin.BindUObject(this, &ThisClass::OnNotifyBeginReceived);

		AbilityTask_PlayMontage_HumanPtr->ReadyForActivation();
	}
}
