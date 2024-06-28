
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
#include "EquipmentElementComponent.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "Planet.h"
#include "CollisionDataStruct.h"
#include "CharacterAttributesComponent.h"
#include "AbilityTask_TimerHelper.h"
#include "Weapon_HandProtection.h"
#include "UIManagerSubSystem.h"
#include "ProgressTips.h"
#include "Skill_Active_Base.h"

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
		auto GameplayAbilityTargetData_DashPtr = dynamic_cast<const FGameplayAbilityTargetData_Skill_WeaponHandProtection*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetData_DashPtr)
		{
			WeaponPtr = GameplayAbilityTargetData_DashPtr->WeaponPtr;
			if (GameplayAbilityTargetData_DashPtr->WeaponPtr)
			{
				return;
			}
		}
	}

	check(0);
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

void USkill_WeaponActive_HandProtection::PerformAction()
{
	Super::PerformAction();

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

void USkill_WeaponActive_HandProtection::PerformStopAction()
{
	ExcuteStopStep();
}

void USkill_WeaponActive_HandProtection::ResetPreviousStageActions()
{
	if (InputRangeHelperPtr)
	{
		InputRangeHelperPtr->RemoveFromParent();
		InputRangeHelperPtr = nullptr;
	}

	Super::ResetPreviousStageActions();
}

bool USkill_WeaponActive_HandProtection::IsEnd() const
{
	if (CurrentIndex >= 3)
	{
		return true;
	}

	return Super::IsEnd();
}

void USkill_WeaponActive_HandProtection::ExcuteStopStep()
{
	IncrementListLock();

	InputRangeHelperPtr = UUIManagerSubSystem::GetInstance()->ViewProgressTips(true);
	InputRangeHelperPtr->SetWaitTime(InputRangeInSecond);

	auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);

	TaskPtr->SetDuration(InputRangeInSecond);

	TaskPtr->OnFinished.BindLambda([this](UAbilityTask_TimerHelper* TaskPtr) {
		CurrentIndex = 0;

		if (InputRangeHelperPtr)
		{
			InputRangeHelperPtr->RemoveFromParent();
			InputRangeHelperPtr = nullptr;
		}

		DecrementListLockOverride();
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
		MakeDamage();

		SkillState = EType::kAttackingEnd;
		if (!bIsRequstCancel)
		{
			DecrementToZeroListLock();
		}
	}
}

void USkill_WeaponActive_HandProtection::MakeDamage()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(PawnECC);

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

		FGameplayEventData Payload;
		Payload.TargetData.Add(GAEventDataPtr);

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

		auto SkillsAry = CharacterPtr->GetEquipmentItemsComponent()->GetSkills();
		auto GASPtr = CharacterPtr->GetAbilitySystemComponent();
		for (const auto& Iter : SkillsAry)
		{
			if (Iter.Value->SkillUnit)
			{
				switch (Iter.Value->SkillUnit->SkillType)
				{
				case ESkillType::kActive:
				{
					for (const auto SkillHandleIter : Iter.Value->HandleAry)
					{
						auto GAPtr = GASPtr->FindAbilitySpecFromHandle(SkillHandleIter);
						if (!GAPtr)
						{
							continue;
						}
						auto GAInstPtr = Cast<USkill_Active_Base>(GAPtr->GetPrimaryInstance());
						if (!GAInstPtr)
						{
							continue;
						}

						GAInstPtr->AddCooldownConsumeTime(1.f);
					}
				}
				break;
				}
			}
		}

		SendEvent(Payload);
	}
}

void USkill_WeaponActive_HandProtection::PlayMontage()
{
	const auto GAPerformSpeed = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().GAPerformSpeed.GetCurrentValue();
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
			CharacterPtr->GetMesh()->GetAnimInstance(),
			Rate
		);

		AbilityTask_PlayMontage_HumanPtr->Ability = this;
		AbilityTask_PlayMontage_HumanPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());
		AbilityTask_PlayMontage_HumanPtr->OnCompleted.BindUObject(this, &ThisClass::DecrementListLockOverride);
		AbilityTask_PlayMontage_HumanPtr->OnInterrupted.BindUObject(this, &ThisClass::DecrementListLockOverride);

		AbilityTask_PlayMontage_HumanPtr->OnNotifyBegin.BindUObject(this, &ThisClass::OnNotifyBeginReceived);

		AbilityTask_PlayMontage_HumanPtr->ReadyForActivation();

		IncrementListLock();
	}
}
